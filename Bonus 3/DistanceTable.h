#ifndef DISTANCETABLE_H
#define DISTANCETABLE_H

class DistanceTable
{
private:
    int dist[20][20];
    int size;

public:
    DistanceTable() {
        size = 0;
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++)
                dist[i][j] = 0;
        }
    }

    void setSize(int b) {
        size = b;
    }

    void setDistance(int i, int j, int d) {
        dist[i][j] = d;
    }

    int getDistance(int i, int j) {
        return dist[i][j];
    }
};

#endif
