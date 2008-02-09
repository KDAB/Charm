#ifndef OPERATIONS_H_
#define OPERATIONS_H_

/* Define functions that implement the operations of the time sheet processor. */
class CommandLine;

void addTimesheet(const CommandLine& cmd);

void removeTimesheet(const CommandLine& cmd);

#endif /*OPERATIONS_H_*/
