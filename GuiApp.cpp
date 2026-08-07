#include "GuiApp.h"
#include "FileManager/FileManager.h"
#include <commdlg.h>
#include <cstdio>
#include <cstring>
#include <fstream>

#pragma comment(lib, "Comdlg32.lib")

enum ControlId {
    ID_BRANCHES = 101,
    ID_SETUP,
    ID_WRAPUP,
    ID_SENIOR,
    ID_JUNIOR,
    ID_AUTO,
    ID_TRANSFER,
    ID_DISTANCES,
    ID_DOCTORS,
    ID_EVENTS,
    ID_IMPORT,
    ID_RUN,
    ID_INTERACTIVE,
    ID_NEXT,
    ID_SAVE,
    ID_CLEAR,
    ID_RESULTS,
    ID_STATUS,
    ID_HEADER,
    ID_SUBHEADER
};

struct GuiControls {
    HWND branches;
    HWND setup;
    HWND wrapup;
    HWND senior;
    HWND junior;
    HWND autoEscalate;
    HWND doctors;
    HWND events;
    HWND results;
    HWND interactive;
    HWND next;
    HWND status;
    HWND transferCheck;
    HWND distances;
};

GuiControls controls;
HFONT titleFont;
HFONT bodyFont;
HFONT codeFont;
HBRUSH windowBrush;
HBRUSH editBrush;
HBRUSH headerBrush;
HBRUSH cardBrush;
SimulationEngine* interactiveEngine = 0;
FileManager interactiveFiles;
char interactiveOutputPath[MAX_PATH] = "";

HWND makeText(HWND parent, const char* text, int x, int y, int width, int height, int id = 0) {
    return CreateWindowA("STATIC", text, WS_CHILD | WS_VISIBLE, x, y, width, height,
                         parent, (HMENU)(INT_PTR)id, 0, 0);
}

HWND makeEdit(HWND parent, int x, int y, int width, int height, int id, bool multiLine = false) {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
    if (multiLine)
        style |= ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_WANTRETURN;
    return CreateWindowExA(0, "EDIT", "", style, x, y, width, height,
                           parent, (HMENU)(INT_PTR)id, 0, 0);
}

HWND makeButton(HWND parent, const char* text, int x, int y, int width, int id) {
    return CreateWindowA("BUTTON", text, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                         x, y, width, 34, parent, (HMENU)(INT_PTR)id, 0, 0);
}

HWND makeCheckBox(HWND parent, const char* text, int x, int y, int width, int id) {
    return CreateWindowA("BUTTON", text, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                         x, y, width, 24, parent, (HMENU)(INT_PTR)id, 0, 0);
}

void setFont(HWND control, HFONT font) {
    SendMessageA(control, WM_SETFONT, (WPARAM)font, TRUE);
}

void setStatus(const char* text) {
    SetWindowTextA(controls.status, text);
}

void showError(HWND window, const char* text) {
    MessageBoxA(window, text, "Clinic Scheduler", MB_OK | MB_ICONERROR);
}

bool parseDistances(const char* text, int branches, char* outBuffer, size_t outSize) {
    char copy[16384];
    strcpy_s(copy, sizeof(copy), text);

    int expected = branches * branches;
    int count = 0;
    char formatted[16384] = "";

    char* context = 0;
    char* token = strtok_s(copy, " \t\r\n", &context);
    while (token != 0) {
        int val = atoi(token);
        char piece[32];
        sprintf_s(piece, "%d ", val);
        strcat_s(formatted, sizeof(formatted), piece);
        count++;
        if (count % branches == 0)
            strcat_s(formatted, sizeof(formatted), "\n");
        token = strtok_s(0, " \t\r\n", &context);
    }

    if (count != expected)
        return false;

    strcpy_s(outBuffer, outSize, formatted);
    return true;
}

void appendText(char* output, int outputSize, const char* text) {
    strcat_s(output, outputSize, text);
}

void appendWaitingQueue(char* output, int outputSize, const char* label,
                        LinkedQueue<Patient*>& queue) {
    char line[64];
    sprintf_s(line, "%s", label);
    appendText(output, outputSize, line);
    bool hasPatients = false;
    queue.forEach([&](Patient* p) {
        sprintf_s(line, "%s%d", hasPatients ? " -> " : "", p->id);
        appendText(output, outputSize, line);
        hasPatients = true;
    });
    if (!hasPatients)
        appendText(output, outputSize, "Empty");
    appendText(output, outputSize, "\r\n");
}

void appendWaitingQueue(char* output, int outputSize, const char* label,
                        PriorityQueue<Patient*>& queue) {
    char line[64];
    sprintf_s(line, "%s", label);
    appendText(output, outputSize, line);
    bool hasPatients = false;
    queue.forEach([&](Patient* p) {
        sprintf_s(line, "%s%d", hasPatients ? " -> " : "", p->id);
        appendText(output, outputSize, line);
        hasPatients = true;
    });
    if (!hasPatients)
        appendText(output, outputSize, "Empty");
    appendText(output, outputSize, "\r\n");
}

void showSnapshot(SimulationEngine& engine, int timeStep) {
    char snapshot[32768] = "";
    char line[256];
    sprintf_s(line, "TIMESTEP %d\r\n\r\n", timeStep);
    appendText(snapshot, sizeof(snapshot), line);

    for (int i = 0; i < engine.numBranches; i++) {
        Branch& b = engine.branches[i];
        sprintf_s(line, "Branch %d\r\n", b.branchNum);
        appendText(snapshot, sizeof(snapshot), line);

        for (int d = 0; d < b.doctorCount; d++) {
            Doctor& doctor = b.doctors[d];
            if (doctor.currentPatient != 0)
                sprintf_s(line, "  Doctor %d: Busy with patient %d until time %d\r\n",
                          doctor.id, doctor.currentPatient->id, doctor.busyUntil);
            else if (timeStep < doctor.shiftStart)
                sprintf_s(line, "  Doctor %d: Shift not started yet\r\n", doctor.id);
            else if (timeStep < doctor.breakUntil)
                sprintf_s(line, "  Doctor %d: On break until time %d\r\n",
                          doctor.id, doctor.breakUntil);
            else
                sprintf_s(line, "  Doctor %d: Free\r\n", doctor.id);
            appendText(snapshot, sizeof(snapshot), line);
        }

        appendWaitingQueue(snapshot, sizeof(snapshot), "  Emergency Queue: ",
                           b.emergencyWaiting);
        appendWaitingQueue(snapshot, sizeof(snapshot), "  Regular Queue: ",
                           b.regularWaiting);
        appendText(snapshot, sizeof(snapshot), "  In-Visit Patients: ");
        bool hasVisits = false;
        for (int d = 0; d < b.doctorCount; d++) {
            if (b.doctors[d].currentPatient != 0) {
                sprintf_s(line, "%s%d", hasVisits ? ", " : "",
                          b.doctors[d].currentPatient->id);
                appendText(snapshot, sizeof(snapshot), line);
                hasVisits = true;
            }
        }
        if (!hasVisits)
            appendText(snapshot, sizeof(snapshot), "None");
        appendText(snapshot, sizeof(snapshot), "\r\n\r\n");
    }

    sprintf_s(line, "Done patients: %d\r\n", engine.doneList.getCount());
    appendText(snapshot, sizeof(snapshot), line);
    SetWindowTextA(controls.results, snapshot);
}

void finishInteractiveSimulation() {
    if (interactiveEngine == 0)
        return;

    interactiveFiles.writeOutput(*interactiveEngine, interactiveOutputPath);
    EnableWindow(controls.next, FALSE);
    setStatus("Interactive simulation completed. The output file was produced.");
}

void runInteractiveTimeStep() {
    if (interactiveEngine == 0)
        return;

    if (interactiveEngine->runOneTimeStep()) {
        showSnapshot(*interactiveEngine, interactiveEngine->currentTime - 1);
        if (interactiveEngine->isComplete())
            finishInteractiveSimulation();
        else
            setStatus("Snapshot displayed. Select Next timestep to continue.");
    }
    else {
        finishInteractiveSimulation();
    }
}

bool startInteractiveSimulation(HWND window, const char* inputPath, const char* outputPath) {
    if (interactiveEngine != 0) {
        delete interactiveEngine;
        interactiveEngine = 0;
    }

    interactiveEngine = new SimulationEngine;
    if (!interactiveFiles.load(inputPath, *interactiveEngine)) {
        delete interactiveEngine;
        interactiveEngine = 0;
        showError(window, "The input could not be read. Check every doctor and event row.");
        return false;
    }

    strcpy_s(interactiveOutputPath, sizeof(interactiveOutputPath), outputPath);
    EnableWindow(controls.next, TRUE);
    runInteractiveTimeStep();
    return true;
}

bool writeGuiInput(char* path, bool& transferOut) {
    transferOut = (SendMessageA(controls.transferCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
    char value[64];
    GetWindowTextA(controls.branches, value, sizeof(value));
    int branches = atoi(value);
    GetWindowTextA(controls.setup, value, sizeof(value));
    int setup = atoi(value);
    GetWindowTextA(controls.wrapup, value, sizeof(value));
    int wrapup = atoi(value);
    GetWindowTextA(controls.senior, value, sizeof(value));
    int senior = atoi(value);
    GetWindowTextA(controls.junior, value, sizeof(value));
    int junior = atoi(value);
    GetWindowTextA(controls.autoEscalate, value, sizeof(value));
    int autoEscalate = atoi(value);

    if (branches < 1 || branches > 20 || setup < 0 || wrapup < 0 || senior < 0 || junior < 0 || autoEscalate < 0)
        return false;

    char doctorText[16384];
    char eventText[16384];
    GetWindowTextA(controls.doctors, doctorText, sizeof(doctorText));
    GetWindowTextA(controls.events, eventText, sizeof(eventText));

    int doctorsPerBranch[20] = {};
    char formattedDoctors[16384] = "";
    int doctorCount = 0;
    char* context = 0;
    char* line = strtok_s(doctorText, "\r\n", &context);
    while (line != 0) {
        int branch, shift, breakAfter, breakDuration;
        char specialization[16];
        if (sscanf_s(line, "%d %15s %d %d %d", &branch, specialization, (unsigned)_countof(specialization), &shift, &breakAfter, &breakDuration) != 5 ||
            branch < 1 || branch > branches)
            return false;
        char code = 0;
        if (strcmp(specialization, "Senior") == 0)
            code = 'S';
        else if (strcmp(specialization, "Junior") == 0)
            code = 'J';
        else
            return false;
        char formattedRow[128];
        sprintf_s(formattedRow, "%d %c %d %d %d\n", branch, code, shift, breakAfter, breakDuration);
        strcat_s(formattedDoctors, sizeof(formattedDoctors), formattedRow);
        doctorsPerBranch[branch - 1]++;
        doctorCount++;
        line = strtok_s(0, "\r\n", &context);
    }

    if (doctorCount == 0)
        return false;

    int eventCount = 0;
    char formattedEvents[16384] = "";
    context = 0;
    line = strtok_s(eventText, "\r\n", &context);
    while (line != 0) {
        char action[16];
        if (sscanf_s(line, "%15s", action, (unsigned)_countof(action)) != 1)
            return false;
        char formattedRow[128];
        if (strcmp(action, "Check-in") == 0) {
            char patientType[16];
            int time, id, branch, tests;
            if (sscanf_s(line, "%15s %15s %d %d %d %d", action, (unsigned)_countof(action), patientType, (unsigned)_countof(patientType), &time, &id, &branch, &tests) != 6)
                return false;
            char typeCode = strcmp(patientType, "Emergency") == 0 ? 'E' : (strcmp(patientType, "Regular") == 0 ? 'R' : 0);
            if (typeCode == 0)
                return false;
            sprintf_s(formattedRow, "C %c %d %d %d %d\n", typeCode, time, id, branch, tests);
        }
        else if (strcmp(action, "Leave") == 0 || strcmp(action, "Urgent") == 0) {
            int time, id;
            if (sscanf_s(line, "%15s %d %d", action, (unsigned)_countof(action), &time, &id) != 3)
                return false;
            sprintf_s(formattedRow, "%c %d %d\n", strcmp(action, "Leave") == 0 ? 'L' : 'U', time, id);
        }
        else
            return false;
        strcat_s(formattedEvents, sizeof(formattedEvents), formattedRow);
        eventCount++;
        line = strtok_s(0, "\r\n", &context);
    }

    FILE* file = fopen(path, "w");
    if (file == 0)
        return false;

    fprintf(file, "%d %d %d %d %d\n", branches, setup, wrapup, senior, junior);
    for (int i = 0; i < branches; i++)
        fprintf(file, "%d\n", doctorsPerBranch[i]);

    fprintf(file, "%s", formattedDoctors);
    fprintf(file, "%d\n", autoEscalate);
    fprintf(file, "%d\n", eventCount);
    fprintf(file, "%s", formattedEvents);

    if (transferOut) {
        char distText[16384];
        GetWindowTextA(controls.distances, distText, sizeof(distText));
        char formattedDist[16384];
        if (!parseDistances(distText, branches, formattedDist, sizeof(formattedDist))) {
            fclose(file);
            return false;
        }
        fprintf(file, "%s", formattedDist);
    }

    fclose(file);
    return true;
}

void readOutput(const char* path) {
    std::ifstream input(path);
    if (!input) {
        SetWindowTextA(controls.results, "No results were produced.");
        return;
    }

    char output[32768] = "";
    char line[512];
    while (input.getline(line, sizeof(line))) {
        if (strlen(output) + strlen(line) + 3 >= sizeof(output))
            break;
        strcat_s(output, sizeof(output), line);
        strcat_s(output, sizeof(output), "\r\n");
    }
    SetWindowTextA(controls.results, output);
}

bool runSimulation(HWND window, const char* inputPath, const char* outputPath, bool transferOn) {
    SimulationEngine engine;
    FileManager files;
    if (!files.load(inputPath, engine, transferOn)) {
        showError(window, "The input could not be read. Check every doctor and event row.");
        return false;
    }

    engine.run();
    if (!files.writeOutput(engine, outputPath)) {
        showError(window, "The output file could not be written.");
        return false;
    }

    readOutput(outputPath);
    setStatus("Simulation completed successfully.");
    return true;
}

void importInput(HWND window) {
    char path[MAX_PATH] = "";
    OPENFILENAMEA dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = window;
    dialog.lpstrFilter = "Clinic input files (*.txt)\0*.txt\0All files\0*.*\0";
    dialog.lpstrFile = path;
    dialog.nMaxFile = MAX_PATH;
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameA(&dialog))
        return;

    std::ifstream input(path);
    if (!input) {
        showError(window, "The selected file could not be opened.");
        return;
    }

    int branches, setup, wrapup, senior, junior;
    if (!(input >> branches >> setup >> wrapup >> senior >> junior)) {
        showError(window, "The selected file does not use the clinic input format.");
        return;
    }

    char number[32];
    sprintf_s(number, "%d", branches); SetWindowTextA(controls.branches, number);
    sprintf_s(number, "%d", setup); SetWindowTextA(controls.setup, number);
    sprintf_s(number, "%d", wrapup); SetWindowTextA(controls.wrapup, number);
    sprintf_s(number, "%d", senior); SetWindowTextA(controls.senior, number);
    sprintf_s(number, "%d", junior); SetWindowTextA(controls.junior, number);

    int perBranch[20] = {};
    int doctorTotal = 0;
    for (int i = 0; i < branches; i++) {
        input >> perBranch[i];
        doctorTotal += perBranch[i];
    }

    char doctors[16384] = "";
    for (int i = 0; i < doctorTotal; i++) {
        int branch, shift, breakAfter, breakDuration;
        char specialization;
        input >> branch >> specialization >> shift >> breakAfter >> breakDuration;
        char row[128];
        sprintf_s(row, "%d %s %d %d %d\r\n", branch, specialization == 'S' ? "Senior" : "Junior", shift, breakAfter, breakDuration);
        strcat_s(doctors, sizeof(doctors), row);
    }
    SetWindowTextA(controls.doctors, doctors);

    int autoEscalate, eventCount;
    input >> autoEscalate >> eventCount;
    sprintf_s(number, "%d", autoEscalate); SetWindowTextA(controls.autoEscalate, number);

    char events[16384] = "";
    for (int i = 0; i < eventCount; i++) {
        char type;
        input >> type;
        char row[128];
        if (type == 'C') {
            char patientType;
            int time, id, branch, tests;
            input >> patientType >> time >> id >> branch >> tests;
            sprintf_s(row, "Check-in %s %d %d %d %d\r\n", patientType == 'E' ? "Emergency" : "Regular", time, id, branch, tests);
        }
        else {
            int time, id;
            input >> time >> id;
            sprintf_s(row, "%s %d %d\r\n", type == 'L' ? "Leave" : "Urgent", time, id);
        }
        strcat_s(events, sizeof(events), row);
    }
    SetWindowTextA(controls.events, events);
    setStatus("Input file imported. Review it and select Run simulation.");
}

void saveOutput(HWND window) {
    char outputPath[MAX_PATH] = "clinic_output.txt";
    OPENFILENAMEA dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = window;
    dialog.lpstrFilter = "Text files (*.txt)\0*.txt\0All files\0*.*\0";
    dialog.lpstrFile = outputPath;
    dialog.nMaxFile = MAX_PATH;
    dialog.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    dialog.lpstrDefExt = "txt";
    if (!GetSaveFileNameA(&dialog))
        return;

    char inputPath[MAX_PATH];
    GetTempPathA(MAX_PATH, inputPath);
    strcat_s(inputPath, sizeof(inputPath), "clinic_scheduler_gui_input.txt");
    bool transferOn;
    if (!writeGuiInput(inputPath, transferOn)) {
        showError(window, "Fill every setting, doctor row, and event row using the examples shown.");
        return;
    }
    runSimulation(window, inputPath, outputPath, transferOn);
}

void clearForm() {
    if (interactiveEngine != 0) {
        delete interactiveEngine;
        interactiveEngine = 0;
    }
    SetWindowTextA(controls.branches, "");
    SetWindowTextA(controls.setup, "");
    SetWindowTextA(controls.wrapup, "");
    SetWindowTextA(controls.senior, "");
    SetWindowTextA(controls.junior, "");
    SetWindowTextA(controls.autoEscalate, "");
    SetWindowTextA(controls.doctors, "");
    SetWindowTextA(controls.events, "");
    SetWindowTextA(controls.results, "");
    EnableWindow(controls.next, FALSE);
    setStatus("Start a new clinic simulation or import an input file.");
}

LRESULT CALLBACK windowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND) {
        int id = LOWORD(wParam);
        if (id == ID_IMPORT)
            importInput(window);
        else if (id == ID_RUN) {
            char inputPath[MAX_PATH];
            char outputPath[MAX_PATH];
            GetTempPathA(MAX_PATH, inputPath);
            GetTempPathA(MAX_PATH, outputPath);
            strcat_s(inputPath, sizeof(inputPath), "clinic_scheduler_gui_input.txt");
            strcat_s(outputPath, sizeof(outputPath), "clinic_scheduler_gui_output.txt");
            bool transferOn;
            if (!writeGuiInput(inputPath, transferOn))
                showError(window, "Fill every setting, doctor row, and event row using the examples shown.");
            else
                runSimulation(window, inputPath, outputPath, transferOn);
        }
        else if (id == ID_NEXT)
            runInteractiveTimeStep();
        else if (id == ID_SAVE)
            saveOutput(window);
        else if (id == ID_CLEAR)
            clearForm();
        return 0;
    }

    if (message == WM_CTLCOLORSTATIC) {
        HWND control = (HWND)lParam;
        HDC dc = (HDC)wParam;
        if (GetDlgCtrlID(control) == ID_HEADER) {
            SetTextColor(dc, RGB(255, 255, 255));
            SetBkColor(dc, RGB(35, 78, 100));
            return (LRESULT)headerBrush;
        }
        SetTextColor(dc, RGB(226, 232, 240));
        int id = GetDlgCtrlID(control);
        if (id == ID_SUBHEADER || id == ID_STATUS) {
            SetBkColor(dc, RGB(20, 27, 35));
            return (LRESULT)windowBrush;
        }
        SetBkColor(dc, RGB(27, 37, 48));
        return (LRESULT)cardBrush;
    }

    if (message == WM_CTLCOLOREDIT) {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, RGB(241, 245, 249));
        SetBkColor(dc, RGB(35, 46, 58));
        return (LRESULT)editBrush;
    }

    if (message == WM_DRAWITEM) {
        DRAWITEMSTRUCT* item = (DRAWITEMSTRUCT*)lParam;
        bool primaryButton = item->CtlID == ID_RUN;
        COLORREF fillColor = primaryButton ? RGB(52, 122, 132) : RGB(62, 76, 91);
        if (item->itemState & ODS_SELECTED)
            fillColor = primaryButton ? RGB(38, 93, 102) : RGB(45, 56, 69);
        if (item->itemState & ODS_DISABLED)
            fillColor = RGB(82, 96, 109);
        HBRUSH buttonBrush = CreateSolidBrush(fillColor);
        HBRUSH borderBrush = CreateSolidBrush(primaryButton ? RGB(134, 192, 196) : RGB(120, 140, 153));
        HRGN buttonShape = CreateRoundRectRgn(item->rcItem.left, item->rcItem.top,
                                               item->rcItem.right + 1, item->rcItem.bottom + 1,
                                               10, 10);
        FillRgn(item->hDC, buttonShape, buttonBrush);
        FrameRgn(item->hDC, buttonShape, borderBrush, 1, 1);
        SetBkMode(item->hDC, TRANSPARENT);
        SetTextColor(item->hDC, RGB(255, 255, 255));
        SelectObject(item->hDC, bodyFont);
        char text[128];
        GetWindowTextA(item->hwndItem, text, sizeof(text));
        DrawTextA(item->hDC, text, -1, &item->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        DeleteObject(buttonShape);
        DeleteObject(buttonBrush);
        DeleteObject(borderBrush);
        return TRUE;
    }

    if (message == WM_PAINT) {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(window, &paint);
        RECT area;
        GetClientRect(window, &area);
        FillRect(dc, &area, windowBrush);
        RECT leftCard = { 14, 100, 310, 665 };
        RECT middleCard = { 320, 100, 790, 665 };
        RECT rightCard = { 800, 100, 1220, 665 };
        FillRect(dc, &leftCard, cardBrush);
        FillRect(dc, &middleCard, cardBrush);
        FillRect(dc, &rightCard, cardBrush);
        HBRUSH cardBorder = CreateSolidBrush(RGB(57, 71, 86));
        FrameRect(dc, &leftCard, cardBorder);
        FrameRect(dc, &middleCard, cardBorder);
        FrameRect(dc, &rightCard, cardBorder);
        DeleteObject(cardBorder);
        EndPaint(window, &paint);
        return 1;
    }

    if (message == WM_DESTROY) {
        DeleteObject(titleFont);
        DeleteObject(bodyFont);
        DeleteObject(codeFont);
        DeleteObject(windowBrush);
        DeleteObject(editBrush);
        DeleteObject(headerBrush);
        DeleteObject(cardBrush);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(window, message, wParam, lParam);
}

void createInterface(HWND window) {
    titleFont = CreateFontA(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                            DEFAULT_PITCH, "Segoe UI");
    bodyFont = CreateFontA(19, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           DEFAULT_PITCH, "Segoe UI");
    codeFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           FIXED_PITCH, "Consolas");
    windowBrush = CreateSolidBrush(RGB(20, 27, 35));
    editBrush = CreateSolidBrush(RGB(35, 46, 58));
    headerBrush = CreateSolidBrush(RGB(35, 78, 100));
    cardBrush = CreateSolidBrush(RGB(27, 37, 48));

    HWND header = CreateWindowA("STATIC", "CLINIC SCHEDULER",
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, 0, 0, 1240, 64,
        window, (HMENU)(INT_PTR)ID_HEADER, 0, 0);
    setFont(header, titleFont);
    HWND subheader = makeText(window, "Build a clinic simulation, validate every detail, and review the schedule instantly.", 24, 69, 760, 24, ID_SUBHEADER);
    setFont(subheader, bodyFont);

    HWND settings = makeText(window, "1. Clinic settings", 24, 114, 260, 24);
    setFont(settings, bodyFont);
    HWND settingsHint = makeText(window, "One tick is one step in the simulation. Enter 2 for two steps.", 24, 140, 280, 36);
    setFont(settingsHint, bodyFont);
    const char* labels[] = { "Number of branches", "Setup time (ticks)", "Wrap-up time (ticks)", "Senior test time (ticks)", "Junior test time (ticks)", "Auto-escalate after (ticks)" };
    HWND* fields[] = { &controls.branches, &controls.setup, &controls.wrapup, &controls.senior, &controls.junior, &controls.autoEscalate };
    int ids[] = { ID_BRANCHES, ID_SETUP, ID_WRAPUP, ID_SENIOR, ID_JUNIOR, ID_AUTO };
    for (int i = 0; i < 6; i++) {
        HWND label = makeText(window, labels[i], 24, 178 + i * 54, 180, 22);
        setFont(label, bodyFont);
        *fields[i] = makeEdit(window, 205, 174 + i * 54, 76, 30, ids[i]);
        setFont(*fields[i], bodyFont);
    }

    controls.transferCheck = CreateWindowA("BUTTON", "Enable branch transfer",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 24, 470, 260, 22, window, (HMENU)(INT_PTR)ID_TRANSFER, 0, 0);
    setFont(controls.transferCheck, bodyFont);

    HWND distLabel = makeText(window, "Branch distances (rows, space-separated)", 24, 498, 270, 22);
    setFont(distLabel, bodyFont);
    controls.distances = makeEdit(window, 24, 522, 260, 80, ID_DISTANCES, true);
    setFont(controls.distances, codeFont);

    HWND doctorTitle = makeText(window, "2. Doctors", 330, 114, 180, 24);
    HWND doctorHelp = makeText(window, "Enter: Branch | Role | Shift starts | Patients before break | Break length", 330, 142, 455, 22);
    HWND doctorColumns = makeText(window, "Example: 1 Senior 8 5 2  = Senior in Branch 1, starts at 8, breaks after 5 visits for 2 ticks.", 330, 168, 455, 40);
    setFont(doctorTitle, bodyFont); setFont(doctorHelp, bodyFont); setFont(doctorColumns, codeFont);
    controls.doctors = makeEdit(window, 330, 212, 450, 139, ID_DOCTORS, true);
    setFont(controls.doctors, codeFont);

    HWND eventTitle = makeText(window, "3. Patient events", 330, 380, 220, 24);
    HWND eventHelp = makeText(window, "Check-in: Action | Patient type | Time | Patient ID | Branch | Number of tests", 330, 408, 455, 22);
    HWND eventColumns = makeText(window, "Example: Check-in Regular 7 1 1 5 = Patient 1 arrives at Branch 1 at time 7 and needs 5 tests.", 330, 434, 455, 46);
    setFont(eventTitle, bodyFont); setFont(eventHelp, bodyFont); setFont(eventColumns, codeFont);
    controls.events = makeEdit(window, 330, 482, 450, 169, ID_EVENTS, true);
    setFont(controls.events, codeFont);

    HWND resultTitle = makeText(window, "4. Schedule results", 810, 114, 250, 24);
    HWND resultHelp = makeText(window, "Calculated automatically from your clinic data.", 810, 142, 380, 22);
    setFont(resultTitle, bodyFont);
    setFont(resultHelp, bodyFont);
    controls.results = makeEdit(window, 810, 174, 400, 477, ID_RESULTS, true);
    SendMessageA(controls.results, EM_SETREADONLY, TRUE, 0);
    setFont(controls.results, codeFont);

    controls.interactive = makeCheckBox(window, "Interactive snapshots (step by step)", 24, 620, 290, ID_INTERACTIVE);
    setFont(controls.interactive, bodyFont);
    HWND importButton = makeButton(window, "Load .txt file", 24, 680, 145, ID_IMPORT);
    HWND runButton = makeButton(window, "Calculate schedule", 179, 680, 175, ID_RUN);
    HWND saveButton = makeButton(window, "Export result", 364, 680, 145, ID_SAVE);
    HWND clearButton = makeButton(window, "New simulation", 519, 680, 130, ID_CLEAR);
    controls.next = makeButton(window, "Next timestep", 664, 680, 130, ID_NEXT);
    EnableWindow(controls.next, FALSE);
    setFont(importButton, bodyFont); setFont(runButton, bodyFont); setFont(saveButton, bodyFont); setFont(clearButton, bodyFont); setFont(controls.next, bodyFont);

    controls.status = makeText(window, "Start a new clinic simulation or import an input file.", 24, 730, 1150, 24, ID_STATUS);
    setFont(controls.status, bodyFont);
}

int runGui(HINSTANCE instance, int showCommand) {
    WNDCLASSA windowClass = {};
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "ClinicSchedulerWindow";
    windowClass.lpfnWndProc = windowProcedure;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    RegisterClassA(&windowClass);

    HWND window = CreateWindowA("ClinicSchedulerWindow", "Clinic Scheduler", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 1250, 800, 0, 0, instance, 0);
    createInterface(window);
    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message;
    while (GetMessageA(&message, 0, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return (int)message.wParam;
}
