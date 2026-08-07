# Clinic Scheduler

Build and run `Clinic.sln` in Visual Studio. The application opens as a Windows desktop interface.

You can either enter the clinic data directly or click **Import input file** to load the existing project input format. Review or edit imported values, then click **Run simulation**. Results appear in the right panel. Use **Save output as...** when you need an output text file.

For direct entry, use one doctor per line:

```txt
Branch  S/J  ShiftStart  BreakAfter  BreakDuration
1 S 1 10 2
1 J 4 20 4
```

Use one event per line:

```txt
C R 7 1 1 5
C E 9 2 1 3
L 15 1
U 22 3
```

`C` means check-in, `L` means leave before service, and `U` means escalate a waiting regular patient to emergency. Event lines must remain ordered by timestamp.
