#include <iostream>
#include <malloc.h>
#include <fstream>
#include <queue>


std::ifstream in("input.txt");
std::ofstream out("output.txt");


unsigned int invertionAmount(unsigned int array[], unsigned int dimension);
unsigned int absorbDiference(unsigned int term1, unsigned int term2);
void swap(unsigned int& term1, unsigned int& term2);


struct State
{
    unsigned int rowOrColumn;
    unsigned int* matrix;
    unsigned int cost = 0;
    unsigned int depth = 0;


    unsigned int manhattenDistance()
    {
        unsigned int cost = 0, aux, auxHorizontal, auxVertical, indexHorizontal, indexVertical;
        for (unsigned int index = 0;index < rowOrColumn * rowOrColumn;index++)
        {
            if (!matrix[index])
                continue;
            aux = matrix[index] - 1;
            auxHorizontal = aux % rowOrColumn;
            auxVertical = aux / rowOrColumn;
            indexHorizontal = index % rowOrColumn;
            indexVertical = index / rowOrColumn;
            cost += absorbDiference(auxHorizontal, indexHorizontal) + absorbDiference(indexVertical, auxVertical);
        }
        return cost;
    }


    void freeMemory()
    {
        free(matrix);
    }


    void createInitialState()
    {
        in >> rowOrColumn;
        matrix = (unsigned int*)malloc(rowOrColumn * rowOrColumn * sizeof(unsigned int));
        for (unsigned int index = 0; index < rowOrColumn * rowOrColumn; index++)
            in >> matrix[index];
        cost = manhattenDistance();
    }


    unsigned int emptyPosition()
    {
        for (unsigned int index = 0;index < rowOrColumn * rowOrColumn;index++)
            if (!matrix[index])
                return index;
        return 0;
    }


    bool isSolvable()
    {
        bool* appearenceArray;
        unsigned int nrInvertions;
        appearenceArray = (bool*)malloc(rowOrColumn * rowOrColumn * sizeof(bool));

        for (unsigned int index = 0; index < rowOrColumn * rowOrColumn; index++)
        {
            appearenceArray[index] = false;
        }
        for (unsigned int index = 0; index < rowOrColumn * rowOrColumn; index++)
        {
            if (appearenceArray[matrix[index]] || matrix[index] >= rowOrColumn * rowOrColumn)
            {
                free(appearenceArray);
                return false;
            }
            appearenceArray[matrix[index]] = true;
        }
        free(appearenceArray);

        nrInvertions = invertionAmount(matrix, rowOrColumn * rowOrColumn);
        if ((rowOrColumn % 2 != 0 && nrInvertions % 2 != 0) || (rowOrColumn % 2 == 0 && (nrInvertions % 2 == 0) != ((rowOrColumn * rowOrColumn - emptyPosition()) % 2 == 0)))
            return false;
        return true;
    }


    State generateUp()
    {
        State newState;
        newState.rowOrColumn = rowOrColumn;
        newState.matrix = (unsigned int*)malloc(rowOrColumn * rowOrColumn * sizeof(unsigned int));
        for (unsigned int index = 0;index < rowOrColumn * rowOrColumn;index++)
            newState.matrix[index] = matrix[index];
        swap(newState.matrix[emptyPosition()], newState.matrix[emptyPosition() - rowOrColumn]);
        newState.cost = newState.manhattenDistance();
        newState.depth = depth + 1;
        return newState;
    }


    State generateRight()
    {
        State newState;
        newState.rowOrColumn = rowOrColumn;
        newState.matrix = (unsigned int*)malloc(rowOrColumn * rowOrColumn * sizeof(unsigned int));
        for (unsigned int index = 0;index < rowOrColumn * rowOrColumn;index++)
            newState.matrix[index] = matrix[index];
        swap(newState.matrix[emptyPosition()], newState.matrix[emptyPosition() + 1]);
        newState.cost = newState.manhattenDistance();
        newState.depth = depth + 1;
        return newState;
    }


    State generateDown()
    {
        State newState;
        newState.rowOrColumn = rowOrColumn;
        newState.matrix = (unsigned int*)malloc(rowOrColumn * rowOrColumn * sizeof(unsigned int));
        for (unsigned int index = 0;index < rowOrColumn * rowOrColumn;index++)
            newState.matrix[index] = matrix[index];
        swap(newState.matrix[emptyPosition()], newState.matrix[emptyPosition() + rowOrColumn]);
        newState.cost = newState.manhattenDistance();
        newState.depth = depth + 1;
        return newState;
    }


    State generateLeft()
    {
        State newState;
        newState.rowOrColumn = rowOrColumn;
        newState.matrix = (unsigned int*)malloc(rowOrColumn * rowOrColumn * sizeof(unsigned int));
        for (unsigned int index = 0;index < rowOrColumn * rowOrColumn;index++)
            newState.matrix[index] = matrix[index];
        swap(newState.matrix[emptyPosition()], newState.matrix[emptyPosition() - 1]);
        newState.cost = newState.manhattenDistance();
        newState.depth = depth + 1;
        return newState;
    }
};



struct StateNod
{
    State state;
    StateNod* shifts[4] = { NULL , NULL, NULL, NULL };  //up, right, down, left
    StateNod* parent = NULL;
    unsigned int action = 51;
    unsigned int provenience = 28;


    void showStateNod()
    {
        out << "State #" << state.depth << ": Array=[ ";
        for (unsigned int index = 0;index < state.rowOrColumn * state.rowOrColumn - 1;index++)
            out << state.matrix[index] << ", ";
        out << state.matrix[state.rowOrColumn * state.rowOrColumn - 1] << " ] Cost: " << state.cost << "      Action: ";
        switch (action)
        {
        case 0:
        {
            out << "Up";
            break;
        }
        case 1:
        {
            out << "Right";
            break;
        }
        case 2:
        {
            out << "Down";
            break;
        }
        case 3:
        {
            out << "Left";
            break;
        }
        case 5:
        {
            out << "HALT";
            break;
        }
        default:
        {
            out << "ERROR";
            break;
        }
        }
        out << "\n";
    }


    void generateNextLevel()
    {
        unsigned int emptySpacePosition = state.emptyPosition();

        if (emptySpacePosition >= state.rowOrColumn && provenience != 2)
        {
            shifts[0] = new StateNod;
            shifts[0]->state = state.generateUp();
            shifts[0]->parent = this;
            shifts[0]->provenience = 0;
        }

        if (emptySpacePosition % state.rowOrColumn != state.rowOrColumn - 1 && provenience != 3)
        {
            shifts[1] = new StateNod;
            shifts[1]->state = state.generateRight();
            shifts[1]->parent = this;
            shifts[1]->provenience = 1;
        }

        if (emptySpacePosition < state.rowOrColumn * (state.rowOrColumn - 1) && provenience != 0)
        {
            shifts[2] = new StateNod;
            shifts[2]->state = state.generateDown();
            shifts[2]->parent = this;
            shifts[2]->provenience = 2;
        }

        if (emptySpacePosition % state.rowOrColumn && provenience != 1)
        {
            shifts[3] = new StateNod;
            shifts[3]->state = state.generateLeft();
            shifts[3]->parent = this;
            shifts[3]->provenience = 3;
        }

    }


    void genrateInitialStateNod(State inputState)
    {
        state = inputState;
        this->generateNextLevel();
    }

};



struct compareStructNod
{
    bool operator()(const StateNod* nod1, const StateNod* nod2)
    {
        return (nod1->state.cost + nod1->state.depth > nod2->state.cost + nod2->state.depth);
    }
};


void showSolution(StateNod* start);
void solvePuzzle(StateNod* initialNod);


int main()
{
    State initial;
    StateNod* initialNod = new StateNod;
    initial.createInitialState();
    initialNod->genrateInitialStateNod(initial);
    solvePuzzle(initialNod);
    return 0;
}


unsigned int invertionAmount(unsigned int array[], unsigned int dimension)
{
    unsigned int ammount = 0;
    for (unsigned int index = 0;index < dimension - 1; index++)
        for (unsigned int jndex = index + 1;jndex < dimension; jndex++)
            if (array[index] > array[jndex] && array[jndex])
                ammount++;
    return ammount;
}


unsigned int absorbDiference(unsigned int term1, unsigned int term2)
{
    if (term1 > term2)
        return term1 - term2;
    return term2 - term1;
}


void swap(unsigned int& term1, unsigned int& term2)
{
    unsigned int aux;
    aux = term1;
    term1 = term2;
    term2 = aux;
}


void solvePuzzle(StateNod* initialNod)
{
    if (!initialNod->state.isSolvable())
    {
        initialNod->showStateNod();
        initialNod->state.freeMemory();
        delete initialNod;
        out << "\nThis puzzle is unsolvable!";
        return;
    }
    if (initialNod->state.cost == 0)
    {
        initialNod->action = 5;
        initialNod->showStateNod();
        initialNod->state.freeMemory();
        delete initialNod;
        return;
    }

    std::priority_queue<StateNod*, std::vector<StateNod*>, compareStructNod> toDo;

    toDo.push(initialNod);

    while (!toDo.empty())
    {
        StateNod* currentNod = toDo.top();
        toDo.pop();
        if (!currentNod->state.cost)
        {
            currentNod->action = 5;
            showSolution(currentNod);
            while (!toDo.empty())
            {
                currentNod = toDo.top();
                toDo.pop();
                currentNod->state.freeMemory();
                delete currentNod;
            }
            return;
        }
        currentNod->generateNextLevel();
        for (int index = 0;index < 4;index++)
        {
            if (currentNod->shifts[index])
                toDo.push(currentNod->shifts[index]);
        }
    }
}


void showSolution(StateNod* start)
{
    while (start->parent)
    {
        start->parent->action = start->provenience;
        start = start->parent;
    }

    while (start->action < 4)
    {
        start->showStateNod();
        start = start->shifts[start->action];
        start->parent->state.freeMemory();
        delete start->parent;
    }
    start->showStateNod();
    start->state.freeMemory();
    delete start;
}