#include <stdio.h>

#include "Cell.h"
#include "CellStack.h"
#include "History.h"

void h_initialize(struct History *hist, int stm, struct Cell *origin, bool firstTime) {

    // Set the short term memory
    hist->m_stm = stm;
    
    hist->m_size = 0;

    if (firstTime) {
        hist->m_stacks.front = NULL;
        hist->m_stacks.back = NULL;
        hist->m_checkpointStack = NULL;
        // TODO: Finish this for other data structures.
    } else {

        // Erase any left-over data
        while (!isEmpty(hist->m_stacks)) {
            pop(hist->m_stacks);
        }
        while (!hist->m_stackReferenceCounts.empty()) {
            hist->m_stackReferenceCounts.pop_front();
        }
        while (!hist->m_modifiedCells.empty()) {
            hist->m_modifiedCells.pop_front();
        }
        if (hist->m_checkpointStack != NULL) {
            destroyStack(hist->m_checkpointStack);
            hist->m_checkpointStack = NULL;
        }
    }

    // Push the stack [(0,0)] since (0,0) or is *always* our first target
    struct CellStack *temp = newStack();
    push(temp,origin);
    push(hist->m_stacks,temp);
    hist->m_stackReferenceCounts.push_back(1);

    // Set the checkpoint values
    hist->m_checkpointCell = origin;
    hist->m_checkpointStack = temp;

}

struct Cell * getCheckpointCell(struct History *hist) {

    // The checkpoint cell is the prev cell for the cell on top of the stack for the 
    // frontmost (oldest) stack that we are storing within the hist->m_stacks queue.
    // Note that although we will have the same checkpoint cell if the top of
    // the stack contains either the origin (0,0) or a neighbor of the origin, but
    // these two situations are differentiated from one another by hist->m_checkpointStack
    // itself.
    
    return hist->m_checkpointCell;

}

struct CellStack * getCheckpointPath(struct History *hist) {

    // Returns a path from the origin to the checkpoint (exclusive) as a stack

    struct CellStack *path = newStack();
    struct Cell *runner = hist->m_checkpointCell;

    while (runner != NULL) {
        push(path,runner);
        runner = runner->m_prev; 
    }

    // The path always includes the origin, and since we can't and don't want 
    // to try to move to the origin, we can *always* pop it off safely
    pop(path);

    return path;

}

struct CellStack * getCheckpointStack(struct History *hist) {

    return copyOfStack(hist->m_checkpointStack);

}

void moved(struct History *hist) {

    hist->m_size++;

    // Assertion - Neither hist->m_stacks nor hist->m_stackReferenceCounts should ever be empty
    if (hist->m_stackReferenceCounts.empty() || isEmpty(hist->m_stacks)) {
        printf("Error - History object has zero stack references\n");
        exit(0);
    }

    // Increment our stack reference counts
    int srefCount = hist->m_stackReferenceCounts.back();
    hist->m_stackReferenceCounts.pop_back();
    hist->m_stackReferenceCounts.push_back(srefCount+1);

    // Every move we push an empty list onto the list of lists of modified cells
    std::list<Cellmod> empty;
    hist->m_modifiedCells.push_front(empty);

    // If the size of the path is larger than our short term memory (which it needn't
    // be) then reduce the reference counts and pop the appropriate number of things
    // off of the stacks.
    if (hist->m_size > hist->m_stm) {

        hist->m_size--;

        int srefCount2 = hist->m_stackReferenceCounts.front();
        hist->m_stackReferenceCounts.pop_front();
        hist->m_stackReferenceCounts.push_front(srefCount2-1);
    }
    if (hist->m_stackReferenceCounts.front() == 0) {
        hist->m_stackReferenceCounts.pop_front();
        pop(hist->m_stacks);
        destroyStack(hist->m_checkpointStack);
        hist->m_checkpointStack = front(hist->m_stacks);
        if (hist->m_checkpointStack.top()->getPrev() != NULL) {
            hist->m_checkpointCell = hist->m_checkpointStack.top()->getPrev();
        }
        else {
            printf("Error - checkpoint has NULL prev Cell\n");
            exit(0);
        }
    }

    // Also make sure to only keep the correct number of modified cells
    if (hist->m_modifiedCells.size() > hist->m_stm) {
        hist->m_modifiedCells.pop_back();
    }
}

void stackUpdate(struct History *hist, struct CellStack *newStack) { // JIMMY CHANGED PARAMETER TO POINTER

    // Assertion - Neither hist->m_stacks nor hist->m_stackReferenceCounts should ever be empty
    if (hist->m_stackReferenceCounts.empty() || isEmpty(hist->m_stacks)) {
        printf("Error - History object has zero stack references\n");
        exit(0);
    }

    // When we perform the stack update, it's always after a cell has been pushed
    // to the path queue. However, in pushing the new Cell, we incremented the
    // reference count for that particular stack. Thus we need to decrement it
    // and then push the proper stack on hist->m_stacks, as well as a reference count
    // of 1 for the new stack on the stack reference count queue
    push(hist->m_stacks,newStack);

    int temp = hist->m_stackReferenceCounts.back();
    hist->m_stackReferenceCounts.pop_back();
    hist->m_stackReferenceCounts.push_back(temp-1);
    hist->m_stackReferenceCounts.push_back(1);
}

void modifiedCellsUpdate(struct History *hist, std::list<Cellmod> *cells) {

    // The only situation when this list will be empty is immediately after
    // returning to the origin after undo is called. This is because the checkpoint
    // stack will contain the origin, and thus won't move from the origin to get to
    // the target (the origin) but WILL perform the appropriate updates.
    if (hist->m_modifiedCells.size() > 0) {
        hist->m_modifiedCells.pop_front();
    }
    hist->m_modifiedCells.push_front(cells);
}

void resetModifiedCells(struct History *hist) {

    std::list<std::list<Cellmod>> temp = hist->m_modifiedCells;

    // Iterate through all modified cells, starting with most recent and going
    // to least recent. During iterations, we simply restore the old values
    while (!temp.empty()) {

        std::list<Cellmod> cellList = temp.front();
        temp.pop_front();

        for (std::list<Cellmod>::iterator it = cellList.begin(); it != cellList.end() ; ++it) {
            (*it).cell->setPrev((*it).oldPrev);
            (*it).cell->setDistance((*it).oldDist);
            (*it).cell->setExplored((*it).oldExplored);
            (*it).cell->setTraversed((*it).oldTraversed);
            for (int i = 0; i < 4; i++) {
                (*it).cell->setWall(i, (*it).oldWalls[i]);
                (*it).cell->setWallInspected(i, (*it).oldWallsInspected[i]);
            }
        }
    }

    // After we get a new checkpoint, we empty everything
    hist->m_size = 0;
    while (!isEmpty(hist->m_stacks)) {
        pop(hist->m_stacks);
    }
    while (!hist->m_stackReferenceCounts.empty()) {
        hist->m_stackReferenceCounts.pop_front();
    }
    while (!hist->m_modifiedCells.empty()) {
        hist->m_modifiedCells.pop_front();
    }

    push(hist->m_stacks,hist->m_checkpointStack);
    hist->m_stackReferenceCounts.push_back(1);
}
