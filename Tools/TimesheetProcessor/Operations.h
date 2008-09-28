#ifndef OPERATIONS_H_
#define OPERATIONS_H_

/* Define functions that implement the operations of the time sheet processor. */
class CommandLine;

void initializeDatabase(const CommandLine& cmd);

void addTimesheet(const CommandLine& cmd);

void removeTimesheet(const CommandLine& cmd);

void checkOrCreateUser(const CommandLine& cmd);

void exportProjectcodes( const CommandLine& cmd );

#endif /*OPERATIONS_H_*/
