#include <utility> 
#include <iostream>
#include <fstream>
#include <limits>
#include <unistd.h>
#include <vector>
#include <cmath>

using namespace std;

void randGen(vector<string>&, unsigned int, unsigned int, unsigned int);
void addSegment(vector<pair<pair<double, double>, pair<double, double>>>&, vector<pair<pair<double, double>, pair<double, double>>>&, unsigned int, pair<double, double>&);
bool isIntersectWith(pair<pair<double, double>, pair<double, double>>, pair<pair<double, double>, pair<double, double>>);
bool isOverlapedWith(pair<pair<double, double>, pair<double, double>>, pair<pair<double, double>, pair<double, double>>);
bool doesContain(pair<pair<double, double>, pair<double, double>>, pair<double, double>);
double distance(pair<double, double>, pair<double, double>);
double getRandUnsignedInt(unsigned int, unsigned int);
double getRandSignedInt(unsigned int, unsigned int);
string pairToStr(pair<double, double>);

int main(int argc, char** argv) {
    int opt, maxNumOfSt = 10, maxNumOfSeg = 5, maxWaitTime = 5, maxNumOfCord = 20;
    while ((opt = getopt(argc, argv, "s:n:l:c:")) != EOF) {
        switch (opt) {
            case 's':
                //To do; Add error handling for minimum input values
                if (optarg)maxNumOfSt = atoi(optarg);
                break;
            case 'n':
                if (optarg)maxNumOfSeg = atoi(optarg);
                break;
            case 'l':
                if (optarg)maxWaitTime = atoi(optarg);
                break;
            case 'c':
                if (optarg)maxNumOfCord = atoi(optarg);
                break;
        }
    }
    vector<string> streets;
    vector<string>& streets_ = streets;
    try {
        while (1) {
            randGen(streets_, maxNumOfSt, maxNumOfSeg, maxNumOfCord);
            for (auto it = streets.begin(); it != streets.end(); ++it) {
                cout << "a " << *it << endl;
            }
            cout << "g" << endl;
            int waitTime = getRandUnsignedInt(5, maxWaitTime);
            sleep(waitTime);
            for (auto it = streets.begin(); it != streets.end(); it++) {
                (*it).erase((*it).find_last_of("\"") + 1, string::npos);
                cout << "r " << (*it) << endl;
            }
            streets.clear();
        }
    } catch (string e) {
        cerr << e << endl;
        return 0;
    }
    return 0;
}

void randGen(vector<string>& streets_, unsigned int maxNumOfSt, unsigned int maxNumOfSeg, unsigned int maxNumOfCord) {
    vector<pair<pair<double, double>, pair<double, double>>> segOfStreets;
    vector<pair<pair<double, double>, pair<double, double>>>& segOfStreets_ = segOfStreets;
    int numOfSt = getRandUnsignedInt(2, maxNumOfSt);
    string name = "a";
    for (int i = 0; i < numOfSt; i++) {
        string street = "\"" + name + "\"" + " ";
        if (name.back() != 'z') {
            name.back() += 1;
        } else {
            name += 'a';
        }
        int numOfSeg = getRandUnsignedInt(1, maxNumOfSeg);
        vector<pair<pair<double, double>, pair<double, double>>> segOfStreet;
        vector<pair<pair<double, double>, pair<double, double>>>& segOfStreet_ = segOfStreet;
        pair<double, double> p1 = make_pair(getRandSignedInt(0, maxNumOfCord), getRandSignedInt(0, maxNumOfCord));
        pair<double, double> p2 = make_pair(getRandSignedInt(0, maxNumOfCord), getRandSignedInt(0, maxNumOfCord));
        while (p1 == p2) {
            p2 = make_pair(getRandSignedInt(0, maxNumOfCord), getRandSignedInt(0, maxNumOfCord));
        }
        pair<pair<double, double>, pair<double, double>> s1 = make_pair(p1, p2);
        segOfStreet.push_back(s1);
        segOfStreets.push_back(s1);
        pair<double, double> last = s1.second;
        pair<double, double>& last_ = last;
        street += pairToStr(s1.first) + " " + pairToStr(s1.second);
        for (int j = 1; j < numOfSeg; j++) {
            addSegment(segOfStreets_, segOfStreet_, maxNumOfCord, last_);
            street += " " + pairToStr(last);
        }
        streets_.push_back(street);
    }
}

void addSegment(vector<pair<pair<double, double>, pair<double, double>>>& segOfStreets_, vector<pair<pair<double, double>, pair<double, double>>>& segOfStreet_, unsigned int maxNumOfCord, pair<double, double>& last_) {
    unsigned int failureCounter = 0;
    while (failureCounter <= 25) {
        bool isAdded = true;
        pair<pair<double, double>, pair<double, double>> s1;

        pair<double, double> p2 = make_pair(getRandSignedInt(0, maxNumOfCord), getRandSignedInt(0, maxNumOfCord));
        s1 = make_pair(last_, p2);
        if (last_ == p2) {
            isAdded = false;
        }

        for (auto seg = segOfStreet_.begin(); seg != segOfStreet_.end(); seg++) {
            if (isIntersectWith(s1, *seg)) {
                isAdded = false;
                break;
            }
        }
        for (auto seg = segOfStreets_.begin(); seg != segOfStreets_.end(); seg++) {
            if (isOverlapedWith(s1, *seg)) {
                isAdded = false;
                break;
            }
        }
        if (isAdded) {
            segOfStreet_.push_back(s1);
            segOfStreets_.push_back(s1);
            last_ = s1.second;
            return;
        }
        failureCounter++;
    }
    throw string("Error: failed to generate valid input for 25 simultaneous attempts");
}

bool isIntersectWith(pair<pair<double, double>, pair<double, double>> s1, pair<pair<double, double>, pair<double, double>> s2) {
    double a1 = s1.second.second - s1.first.second;
    double a2 = s2.second.second - s2.first.second;
    double b1 = s1.first.first - s1.second.first;
    double b2 = s2.first.first - s2.second.first;
    double c1 = s1.first.first * s1.second.second - s1.second.first * s1.first.second;
    double c2 = s2.first.first * s2.second.second - s2.second.first * s2.first.second;
    //    Check if they are 2 vertical lines
    if (b1 == 0 && b2 == 0) {
        return false;
    } else if ((b1 != 0 && b2 != 0)&&(a1 / b1 == a2 / b2)) {
        return false;
    } else {
        double det = a1 * b2 - b1 * a2;
        double detX = c1 * b2 - b1 * c2;
        double detY = a1 * c2 - c1 * a2;
        double xi = detX / det;
        double yi = detY / det;
        pair<double, double> intersection = make_pair(xi, yi);
        bool isInSegment = doesContain(s1, intersection) && doesContain(s2, intersection);
        bool isEndPoint = (intersection == s1.first) || (intersection == s1.second);
        return isInSegment && (!isEndPoint);
    }
}

bool isOverlapedWith(pair<pair<double, double>, pair<double, double>> s1, pair<pair<double, double>, pair<double, double>> s2) {
    double a1 = s1.second.second - s1.first.second;
    double a2 = s2.second.second - s2.first.second;
    double b1 = s1.first.first - s1.second.first;
    double b2 = s2.first.first - s2.second.first;
    double c1 = s1.first.first * s1.second.second - s1.second.first * s1.first.second;
    double c2 = s2.first.first * s2.second.second - s2.second.first * s2.first.second;

    double r1 = 0, r2 = 0, r3 = 0;
    if (!(a1 == 0 && a2 == 0)) {
        if (a1 == 0 || a2 == 0) {
            return false;
        }
        r1 = a1 / a2;
    }
    if (!(b1 == 0 && b2 == 0)) {
        if (b1 == 0 || b2 == 0) {
            return false;
        }
        r2 = b1 / b2;
    }
    if (!(c1 == 0 && c2 == 0)) {
        if (c1 == 0 || c2 == 0) {
            return false;
        }
        r3 = c1 / c2;
    }
    if (r1 == r2 || r2 == r3 || r1 == r3) {
        return doesContain(s2, s1.first) || doesContain(s2, s1.second) || doesContain(s1, s2.first) || doesContain(s1, s2.second);
    } else {
        return false;
    }
}

bool doesContain(pair<pair<double, double>, pair<double, double>> s, pair<double, double> p) {
    double epsilon = 0.0000001;
    return abs(distance(s.first, p) + distance(s.second, p) - distance(s.first, s.second)) < epsilon;
}

double distance(pair<double, double> p1, pair<double, double> p2) {
    return sqrt(pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2));
}

double getRandUnsignedInt(unsigned int low, unsigned int high) {
    ifstream urandom("/dev/urandom");
    unsigned int uimax = numeric_limits<unsigned int>::max();
    unsigned int seed = 10;
    urandom.read((char*) &seed, sizeof (unsigned int));
    urandom.close();
    return (double) ((unsigned int) ((double) seed / uimax * (high - low + 1) + low));
}

double getRandSignedInt(unsigned int low, unsigned int high) {
    int sign = (int) getRandUnsignedInt(0, 1) == 0 ? -1 : 1;
    int val = (int) getRandUnsignedInt(low, high);
    return (double) (sign * val);
}

string pairToStr(pair<double, double> p) {
    string x = to_string(p.first);
    if (x.find_last_not_of('0') == x.find('.')) {
        x.erase(x.find_last_not_of('0'), string::npos);
    } else {
        x.erase(x.find_last_not_of('0') + 1, string::npos);
    }
    string y = to_string(p.second);
    if (y.find_last_not_of('0') == y.find('.')) {
        y.erase(y.find_last_not_of('0'), string::npos);
    } else {
        y.erase(x.find_last_not_of('0') + 1, string::npos);
    }
    string out = "(" + x + ", " + y + ")";
    return out;
}

