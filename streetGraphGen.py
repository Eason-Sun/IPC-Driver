from __future__ import print_function
import math
import re
import ast
import sys


def main():
    street = Street()
    graph = Graph(street)
    while True:
        try:
            input = sys.stdin.readline()
            output = inputParser(input, street)
            if input == '':
                sys.exit(0)
            if output[0] == "Error: ":
                print(output[0] + output[1])
            elif output[0] == "a":
                cordList = [ast.literal_eval(elem + ")") for elem in
                            output[2].replace(" ", "").split(")") if elem]
                street.addStreet(output[1], cordList)
            elif output[0] == "c":
                cordList = [ast.literal_eval(elem + ")") for elem in
                            output[2].replace(" ", "").split(")") if elem]
                street.changeStreet(output[1], cordList)
            elif output[0] == "r":
                street.removeStreet(output[1])
            elif output[0] == "g":
                graph = Graph(street)
                graph.graphUpdate()
                sys.stdout.write(graph.graphPrint())
                sys.stdout.flush()
            else:
                pass

        except (KeyboardInterrupt):
            print("\nTerminated.")
            sys.exit(0)


def inputParser(line, street):
    output = list()
    isGen = re.search("^(g)$", line)
    isRemove = re.search("^(r)\s", line)
    isAddOrChange = re.search("^([ac])\s", line)
    isQuated = re.search("^([acr])\s(\".*?\")", line)
    isNonEmptyStreetName = re.search("^[acr]\s\"([a-zA-Z0-9\s]+?)\"", line)
    isValidCords = re.search(
        "^([acr])\s\"([a-zA-Z0-9\s]+?)\"((\s*\(\s*\-?[0-9]+\.?[0-9]*\s*,\s*\-?[0-9]+\.?[0-9]*\s*\))+)", line)

    if "exit" in line:
        output.append("EOF")

    elif line == '\n':
        output.append("p")
    else:
        if isGen:
            output.append("g")
        elif isRemove:
            if isQuated:
                if isNonEmptyStreetName:
                    if not isNonEmptyStreetName.group(1) in street.streets:
                        output.extend(["Error: ", "The street does not exist."])
                    else:
                        output.extend(["r", isNonEmptyStreetName.group(1)])
                else:
                    output.extend(["Error: ", "Invalid street name in your command."])
            else:
                output.extend(["Error: ", "Lack of quotation mark for the street name."])
        elif isAddOrChange:
            if isQuated:
                if isNonEmptyStreetName:
                    if isValidCords:
                        if isValidCords.group(1) == "a":
                            if not isValidCords.group(2) in street.streets:
                                output.extend(["a", isValidCords.group(2), isValidCords.group(3)])
                            else:
                                output.extend(["Error: ", "This street has already been added."])
                        else:
                            if not isValidCords.group(2) in street.streets:
                                output.extend(["Error: ", "The street does not exist."])
                            else:
                                output.extend(["c", isValidCords.group(2), isValidCords.group(3)])
                    else:
                        output.extend(["Error: ", "Invalid coordinate setting in your command."])
                else:
                    output.extend(["Error: ", "Invalid street name in your command."])
            else:
                output.extend(["Error: ", "Lack of quotation mark for the street name."])
        else:
            output.extend(["Error: ", "'a' or 'c' or 'r' specified for a street that does not exist."])
    return output


def pp(x):
    """Returns a pretty-print string representation of a number.
       A float number is represented by an integer, if it is whole,
       and up to two decimal places if it isn't
    """
    if isinstance(x, float):
        if x.is_integer():
            return str(int(x))
        else:
            return "{0:.2f}".format(x)
    return str(x)


class Point(object):
    """A point in a two dimensional space"""

    def __init__(self, pair):
        x, y = pair
        self.x = float(x)
        self.y = float(y)

    def __repr__(self):
        return '(' + pp(self.x) + ', ' + pp(self.y) + ')'

    def __eq__(self, other):
        """Overrides the default implementation"""
        if isinstance(other, Point):
            return self.x == other.x and self.y == other.y
        return False

    def __hash__(self):
        return hash((self.x, self.y))


class Line(object):
    """A line between two points"""

    def __init__(self, src, dst):
        self.src = src
        self.dst = dst

    def __repr__(self):
        return '[' + str(self.src) + '-->' + str(self.dst) + ']'

    def __eq__(self, other):
        """Overrides the default implementation"""
        if isinstance(other, Line):
            return self.src == other.src and self.dst == other.dst
        return False

    def __hash__(self):
        return hash((self.src, self.dst))


class Street(object):

    def __init__(self):
        self.streets = {}
        self.intersectLineDict = {}
        self.lineWithMultiIntersectDict = {}
        self.singleIntersectLines = {}

    def addStreet(self, streetName, cordList):
        self.streets[streetName] = list()
        for elem in cordList:
            self.streets[streetName].append(Point(elem))

    def changeStreet(self, streetName, cordList):
        del self.streets[streetName]
        self.streets[streetName] = list()
        for elem in cordList:
            self.streets[streetName].append(Point(elem))

    def removeStreet(self, streetName):
        del self.streets[streetName]

    def findStreetIntersections(self):
        self.intersectLineDict = {}
        self.lineWithMultiIntersectDict = {}
        self.singleIntersectLines = {}
        streets_segments = self.findStreetSegments()
        # Iterate through all combinations of "len(streets_segments) choose 2"
        for i in range(len(streets_segments)):
            for line1 in streets_segments[i]:
                numOfIntersections = 0
                intersectSet = set()
                for j in range(len(streets_segments)):
                    if not j == i:
                        for line2 in streets_segments[j]:
                            if self.isOverlap(line1, line2):
                                a = self.is_between(line2.src, line1.src, line1.dst)
                                b = self.is_between(line2.dst, line1.src, line1.dst)
                                if a and b:
                                    if not line2.src in self.intersectLineDict:
                                        self.intersectLineDict[line2.src] = set()
                                    if not line2.dst in self.intersectLineDict:
                                        self.intersectLineDict[line2.dst] = set()
                                    self.intersectLineDict[line2.src].add(line1)
                                    self.intersectLineDict[line2.dst].add(line1)
                                    intersectSet.update([line2.src, line2.dst])
                                    numOfIntersections += 2
                                elif a:
                                    if not line2.src in self.intersectLineDict:
                                        self.intersectLineDict[line2.src] = set()
                                    self.intersectLineDict[line2.src].add(line1)
                                    intersectSet.add(line2.src)
                                    numOfIntersections += 1
                                elif b:
                                    if not line2.dst in self.intersectLineDict:
                                        self.intersectLineDict[line2.dst] = set()
                                    self.intersectLineDict[line2.dst].add(line1)
                                    intersectSet.add(line2.dst)
                                    numOfIntersections += 1
                            else:
                                intersection = self.findLineIntersection(line1, line2)
                                if not intersection == None:
                                    if not intersection in self.intersectLineDict:
                                        self.intersectLineDict[intersection] = set()
                                    self.intersectLineDict[intersection].update([line1, line2])
                                    if not intersection in intersectSet:
                                        numOfIntersections += 1
                                    intersectSet.add(intersection)
                if numOfIntersections > 1:
                    self.lineWithMultiIntersectDict[line1] = intersectSet
                elif numOfIntersections == 1:
                    l = list(intersectSet)
                    self.singleIntersectLines[line1] = l[0]

    def findStreetSegments(self):
        # streets_segments has the format of [[(x1, y1),(x2, y2),...],[],...]
        streets_segments = []
        for street in self.streets:
            street_segments = []
            for i in range(len(self.streets.get(street)) - 1):
                street_segments.append(Line(self.streets.get(street)[i], self.streets.get(street)[i + 1]))
            streets_segments.append(street_segments)
        return streets_segments

    def isOverlap(self, line1, line2):
        a1, b1, c1 = self.findCoefficients(line1)
        a2, b2, c2 = self.findCoefficients(line2)
        r1 = r2 = r3 = 0
        count = 0
        if not a1 == a2 == 0:
            if a2 == 0 or a1 == 0:
                return False
            r1 = float(a1) / a2
        if not b1 == b2 == 0:
            if b2 == 0 or b1 == 0:
                return False
            r2 = float(b1) / b2
        if not c1 == c2 == 0:
            if c2 == 0 or c1 == 0:
                return False
            r3 = float(c1) / c2
        return r1 == r2 or r1 == r3 or r2 == r3

    # Find the coefficients for ax + by = c with given line segment
    def findCoefficients(self, line):
        a = line.dst.y - line.src.y
        b = line.src.x - line.dst.x
        c = line.src.x * line.dst.y - line.dst.x * line.src.y
        return a, b, c

    def is_between(self, pointC, pointA, pointB):
        epsilon = 0.0000001

        def distance(pointA, pointB):
            return math.sqrt((pointA.x - pointB.x) ** 2 + (pointA.y - pointB.y) ** 2)

        # Point C resides on Line AB iif AC + BC = AB
        return abs(distance(pointC, pointA) + distance(pointC, pointB) - distance(pointA, pointB)) < epsilon

    def findLineIntersection(self, line1, line2):
        a1, b1, c1 = self.findCoefficients(line1)
        a2, b2, c2 = self.findCoefficients(line2)

        # Check if they are 2 vertical lines
        if b1 == b2 == 0:
            return
        # Check if they are parallel
        elif not (b1 == 0 or b2 == 0) and float(a1) / b1 == float(a2) / b2:
            return
        # Using Cramer's rule to find the intersection
        else:
            det = a1 * b2 - b1 * a2
            detX = c1 * b2 - b1 * c2
            detY = a1 * c2 - c1 * a2
            xi = float(detX) / det
            yi = float(detY) / det
            if math.floor(xi) == xi:
                xi = int(xi)
            if math.floor(yi) == yi:
                yi = int(yi)
            # Check if the intersection point resides on both line segments
            ifInRange = self.is_between(Point((xi, yi)), line1.src, line1.dst) and self.is_between(Point((xi, yi)),
                                                                                                   line2.src, line2.dst)
            if not ifInRange:
                return
            else:
                return Point((xi, yi))


class Graph(object):

    def __init__(self, street):
        self.vertexId = 0
        self.graph = {}
        self.graphWithId = {}
        self.street = street

    def graphUpdate(self):
        if len(self.graph) == 0:
            self.graphGen()
            self.rmDuplicates()
            self.graphWithIdGen()

        else:
            self.graphGen()
            self.rmDuplicates()
            graphWithId = {}
            oldIdConfig = set()
            for intersection in self.graphWithId:
                oldIdConfig.add(intersection)
                for node in self.graphWithId[intersection]:
                    oldIdConfig.add(node)
            newIntersectWithId = set()
            for intersection in self.graph:
                isOldNode = False
                thisId = None
                for oldNode in oldIdConfig:
                    if intersection == oldNode[0]:
                        graphWithId[oldNode] = list()
                        isOldNode = True
                        thisId = oldNode[1]
                        break
                if not isOldNode:
                    isAssigned = False
                    for elem in newIntersectWithId:
                        if intersection == elem[0]:
                            graphWithId[(intersection, elem[1])] = list()
                            thisId = elem[1]
                            isAssigned = True
                            break
                    if not isAssigned:
                        graphWithId[(intersection, self.vertexId)] = list()
                        thisId = self.vertexId
                        self.vertexId += 1
                for node in self.graph[intersection]:
                    isOldNode = False
                    for oldNode in oldIdConfig:
                        if node == oldNode[0]:
                            graphWithId[(intersection, thisId)].append(oldNode)
                            isOldNode = True
                            break
                    if not isOldNode:
                        if node in self.graph:
                            newIntersectWithId.add(((node, self.vertexId)))
                        graphWithId[(intersection, thisId)].append((node, self.vertexId))
                        self.vertexId += 1
            self.graphWithId = graphWithId

    def graphGen(self):
        self.graph = {}
        self.street.findStreetIntersections()
        intersecLineDict = self.street.intersectLineDict
        lineWithMultiIntersectDict = self.street.lineWithMultiIntersectDict
        singleIntersectLines = self.street.singleIntersectLines
        for intersection in intersecLineDict:
            if not intersection in self.graph:
                self.graph[intersection] = list()
            for line in intersecLineDict[intersection]:
                if line in singleIntersectLines:
                    if intersection == line.src:
                        if not line.dst in self.graph[intersection]:
                            self.graph[intersection].append(line.dst)
                    elif intersection == line.dst:
                        if not line.src in self.graph[intersection]:
                            self.graph[intersection].append(line.src)
                    else:
                        if not line.dst in self.graph[intersection]:
                            self.graph[intersection].append(line.dst)
                        if not line.src in self.graph[intersection]:
                            self.graph[intersection].append(line.src)
                else:
                    setOfPoints = set()
                    setOfPoints.update([line.src, line.dst])
                    setOfPoints.update(lineWithMultiIntersectDict[line])
                    l = list(setOfPoints)
                    sortedList = self.sortPoints(l)
                    if intersection == sortedList[0]:
                        self.graph[intersection].append(sortedList[1])
                    elif intersection == sortedList[-1]:
                        self.graph[intersection].append(sortedList[-2])
                    else:
                        last = sortedList[sortedList.index(intersection) - 1]
                        next = sortedList[sortedList.index(intersection) + 1]
                        if not last in self.graph[intersection]:
                            self.graph[intersection].append(last)
                        if not next in self.graph[intersection]:
                            self.graph[intersection].append(next)

    def sortPoints(self, pointList):
        # If it's vertical, sort it by y-val
        sortedPointList = list()
        tupList = list()
        for p in pointList:
            tupList.append((p.x, p.y))
        if pointList[0].x == pointList[1].x:
            sortedTupList = sorted(tupList, key=lambda tup: tup[1])
        # sort it by x-val
        else:
            sortedTupList = sorted(tupList, key=lambda tup: tup[0])
        for tup in sortedTupList:
            sortedPointList.append(Point(tup))
        return sortedPointList

    def rmDuplicates(self):
        for intersection in self.graph:
            for node in self.graph[intersection]:
                if node in self.graph:
                    self.graph[node].remove(intersection)

    def graphWithIdGen(self):
        nodeSet = set()
        nodeSetWithId = list()
        i = 0
        for intersection in self.graph:
            nodeSet.add(intersection)
            for node in self.graph[intersection]:
                nodeSet.add(node)
        for elem in nodeSet:
            nodeSetWithId.append((elem, self.vertexId))
            self.vertexId += 1
        for intersection in self.graph:
            for nodeWithId in nodeSetWithId:
                if intersection == nodeWithId[0]:
                    self.graphWithId[(intersection, nodeWithId[1])] = list()
        for intersection in self.graph:
            intersectionId = -1
            for nodeWithId in self.graphWithId:
                if intersection == nodeWithId[0]:
                    intersectionId = nodeWithId[1]
                    break

            for node in self.graph[intersection]:
                nodeId = -1
                for nodeWithId in nodeSetWithId:
                    if node == nodeWithId[0]:
                        nodeId = nodeWithId[1]
                        break
                self.graphWithId[(intersection, intersectionId)].append((node, nodeId))

    def graphPrint(self):
        outputStr = ""
        maxId = -1
        # nodeWithIdList = list()
        # outputStr += "V = {\n"
        # for intersection in self.graphWithId:
        #     nodeWithIdList.append(intersection)
        #     for node in self.graphWithId[intersection]:
        #         if not (node in self.graphWithId or node in nodeWithIdList):
        #             nodeWithIdList.append(node)
        # nodeWithIdList = sorted(nodeWithIdList, key=lambda tup: tup[1])
        # for vertex in nodeWithIdList:
        #     outputStr += "  {}:  {}\n".format(vertex[1], vertex[0])
        # outputStr += "}\nE = {\n"
        for intersection in self.graphWithId:
            if intersection[1] > maxId:
                maxId = intersection[1]
            for node in self.graphWithId[intersection]:
                if node[1] > maxId:
                    maxId = node[1]
        outputStr += "V {}\n".format(maxId + 1)
        outputStr += "E {"

        for intersection in self.graphWithId:
            for node in self.graphWithId[intersection]:
                outputStr += "<{},{}>,".format(intersection[1], node[1])
        if outputStr[-1] is ',':
            outputStr = outputStr[:-1]
        outputStr += "}\n"
        return outputStr


if __name__ == '__main__':
    main()
