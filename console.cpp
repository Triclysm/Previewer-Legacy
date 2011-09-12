/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *                                 Console Source Code                                 *
 *                                      TRICLYSM                                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  This file contains the main implementation of all console-related functions (but   *
 *  not the console commands themselves), including parsing the actual user's input.   *
 *  Actual console commands are defined in console_commands.cpp.                       *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                     *
 *  Copyright (C) 2011 Brandon Castellano, Ryan Mantha. All rights reserved.           *
 *  Triclysm is provided under the BSD-2-Clause license. This program uses the SDL     *
 *  (Simple DirectMedia Layer) library, and the Lua scripting language. See the        *
 *  included LICENSE file or <http://www.triclysm.com/> for more details.              *
 *                                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \file  console.cpp
/// \brief This file contains the implementation of all console-related functions for the
///        Triclysm program itself, including the command output and command history.
///


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               PREPROCESSOR DIRECTIVES                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "console.h"    // Complimentary header to this source file.
#include "TCCube.h"     // Required for the GetConstantValue function.

#include <string>       // Strings library.
#include <cctype>       // Used for string comparison.
#include <list>         // STL List container.
#include <vector>       // STL Vector container.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                  GLOBAL VARIABLES                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool consoleEnabled;    ///< Set to true to enable the console, false to disable it.

size_t      maxInputLength,     ///< The maximum number of input characters in a command.
            cursorPos;          ///< The current position of the cursor in the input.
std::string currInput;          ///< String containing the user's current input.
bool        historyRetrieved;   ///< True if history has been retrieved, false otherwise.

size_t maxHistoryLines,         ///< The maximum number of history entires to cache.
       maxOutputLines;          ///< The maximum number of output lines to cache.
       
std::list<ConsoleCommand*> cmdList;             ///< The list of console commands.
std::list<CommandAlias*>   aliasList;           ///< The list of command aliases.

std::list<std::string>           outputList,    ///< The list of output lines.
                                 historyList;   ///< The list of history entries.
std::list<std::string>::iterator outputIt,      ///< Output list iterator.
                                 historyIt;     ///< History list iterator.

const std::string inputPrefix = " > ";          ///< Prefix for the current command input.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                FUNCTION DEFINITIONS                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///
/// \brief Initialize Console
///
/// Sets all required console parameters, and registers all console commands defined in
/// console_commands.cpp.
///
/// \param maxInputLen  
/// \param maxHistLines 
/// \param maxOutLines  
///
/// \see maxInputLength | maxHistoryLines | maxOutputLines | RegisterCommands
///
void InitConsole(size_t maxInputLen, size_t maxHistLines, size_t maxOutLines)
{
    maxInputLength  = maxInputLen;
    maxHistoryLines = maxHistLines;
    maxOutputLines  = maxOutLines;
    TCC_COMMANDS::RegisterCommands();
    consoleEnabled = false;
}


///
/// \brief Write Output
///
/// Appends the passed string to the console output list (\ref outputList).  If the number
/// of output entries exceeds the maximum (\ref maxOutputLines), the remaining output
/// lines are discarded.
///
/// \param outputStr The string to append to the output list.
///
void WriteOutput(std::string const& outputStr)
{
	std::string newOutput(outputStr);
	size_t newLinePos = newOutput.find('\n');
	while (newLinePos != std::string::npos)
	{
		outputList.push_front(newOutput.substr(0, newLinePos));
		newOutput.erase(0, newLinePos + 1);
		newLinePos = newOutput.find('\n');
	}
	outputList.push_front(newOutput);
	while (outputList.size() > maxOutputLines)
	{
		outputList.pop_back();
	}
}


///
/// \brief Write History
///
/// Appends the passed string to the console history list (\ref historyList), and resets
/// the history iterator (\ref historyIt) to the beginning of the list.  If the number
/// of history entries exceeds the maximum (\ref maxHistLines), the remaining history
/// entries are discarded.
///
/// \param historyStr The string to append to the history list.
///
void WriteHistory(std::string const& historyStr)
{
    historyList.push_front(historyStr);
    while (historyList.size() > maxHistoryLines) historyList.pop_back();
    historyIt = historyList.begin();
    historyRetrieved = false;
}


///
/// \brief Clear Output
///
/// Clears everything from the \ref outputList.
///
void ClearOutput()
{
    outputList.clear();
}


///
/// \brief Clear History
///
/// Clears everything from the \ref historyList.
///
void ClearHistory()
{
    historyList.clear();
}


///
/// \brief Compare Console Command
///
/// \returns True if the first command comes before the second, false otherwise.
/// \see     cmpCmdAlias
///
bool cmpConsoleCmd(ConsoleCommand const *first, ConsoleCommand const *second)
{
    return cmpStrNoCase(first->name, second->name);
}


///
/// \brief Compare Command Alias
///
/// \returns True if the first alias comes before the second, false otherwise.
/// \see     cmpCmdAlias
///
bool cmpCmdAlias(CommandAlias const *first, CommandAlias const *second)
{
    return cmpStrNoCase(first->alias, second->alias);
}


///
/// \brief Compare String (No Case)
/// 
/// \returns True if the first string comes before the second, false otherwise.
///
bool cmpStrNoCase(std::string const& first, std::string const& second)
{
    // First, we store the length of both strings.
    size_t firstLen  = first.length(),
           secondLen = second.length();
    // Next, looping through every possible character in both strings...
    for (size_t i = 0; (i < firstLen) && (i < secondLen); i++)
    {
        // If the current character in the first is a lower letter then the second...
        if (tolower(first[i]) < tolower(second[i]))
        {
            return true;    // Then we return true (first comes before the the second).
        }
        // Otherwise, if the current character is a higher letter...
        else if (tolower(first[i]) > tolower(second[i]))
        {
            return false;   // Then we return false (second comes before the the first).
        }
    }
    // If we get here, all the characters so far were equal, so we compare lengths.
    if (firstLen < secondLen)   // So, if the second string is longer than the first...
    {
        return true;                // Then the first must come before the second.
    }
    return false;       // If we get here, then the second must come before the first.
}


///
/// \brief Call Command
/// 
/// Parses the passed command string as a console command.  This function will first 
/// try to find the command (or command alias) that is trying to be called.  It then
/// will go through and detect all passed command arguments, and pass these in a vector
/// to the appropriate command in console_commands.cpp.  If the command cannot be found,
/// an error message is written to the console.
///
/// \param cmd The command string to parse as a command.
/// \see   GetCommand
///
void CallCommand(std::string const& cmd)
{
    std::string icmd;   // Holds just the name of the command (not the whole string).
    // First, we need to get the value of icmd.
    if (cmd.find(' ') != std::string::npos)     // So, if there is a space in the string...
    {
        icmd = cmd.substr(0, cmd.find(' '));        // Then get the command name substring.
    }
    else                                        // Else, if there is no space...
    {
        icmd = cmd;                                 // Then cmd is the command name.
    }
    ConsoleCommand *command = GetCommand(icmd);
    if (command != NULL)
    {
        // First, we need to parse the command into its argument vectors.
        std::vector<std::string> argVect;
        bool                     quotesMode = false;
        std::string              currArg;
        for (size_t i = icmd.length(); i < cmd.length(); i++)
        {
            if (cmd[i] == '\"')     // If we have a double quote...
            {
                // Do we have an escape character behind us?
                if ((i - 1) > 0 && cmd[i] == '\\')  // Is the quote escaped?
                {
                    // If so, then just add the quote.
                    currArg.push_back('\"');
                }
                else
                {
                    quotesMode = !quotesMode;       // Otherwise, toggle quotes mode.
                }
            }
            else
            {
                if (cmd[i] == ' ')
                {
                    if (quotesMode)
                    {
                        currArg.push_back(' ');
                    }
                    else if (currArg.length() > 0)
                    {
                        argVect.push_back(currArg);
                        currArg.clear();
                    }
                }
                else
                {
                    currArg.push_back(cmd[i]);
                }
            }
        }
        if (currArg.length() > 0) argVect.push_back(currArg);
        // Lastly, we call the command (passing the arguments vector), and return.
        command->func(argVect);
        return;
    }
    // If we get here, then the command wasn't found, so we show an error to the user.
    WriteOutput("'" + icmd + "' is not a recognized command.");
}


///
/// \brief Scroll History
///
/// Advances or decrements the history list iterator (\ref historyIt) to display either
/// the previous or next history entry
/// 
/// \param next True to scroll to the next history entry (an earlier command), or false
///             to go to the previous entry (a later command).
///
void ScrollHistory(bool next)
{
    if (historyList.size() == 0) return;
    
    if (next)
    {
        if (historyRetrieved)
        {
            historyIt++;
            if (historyIt == historyList.end()) historyIt--;
        }
        else
        {
            historyRetrieved = true;
        }
    }
    else if (historyRetrieved && !next && historyIt != historyList.begin())
    {
        historyIt--;
    }
    currInput = *historyIt;
    cursorPos = currInput.length();
}


///
/// \brief Move Cursor
///
/// Moves the console cursor to the left or right by one character.
///
/// \param left True to move the cursor to the left, false to move it to the right.
///
void MoveCursor(bool left)
{
    if      ( left && cursorPos > 0)                  cursorPos--;
    else if (!left && cursorPos < currInput.length()) cursorPos++;
}


///
/// \brief Parse Input
///
/// Parses the \ref currInput string by stripping all excess whitespace, splitting the
/// string at every command delimiter (';'), and calling \ref CallCommand for every
/// individual command that is passed.
///
/// \remarks This function is called when the user presses Enter in the console.
///
void ParseInput()
{
    // First, we strip all leading and trailing whitespace from the command.
    StripWhitespaceLT(currInput);
    // So, if the remaining string is not blank...
    if (!currInput.empty())
    {
        // We add the line to the console output and command history.
        WriteOutput(inputPrefix + currInput);
        WriteHistory(currInput);
        currInput.push_back(';');
        size_t scPos = currInput.find(';');
        while (scPos != std::string::npos)
        {
            std::string currCmd = currInput.substr(0, scPos);
            StripWhitespaceLT(currCmd);
            if (!currCmd.empty())
            {
                CallCommand(currCmd);
            }
            currInput.erase(0, scPos + 1);
            scPos = currInput.find(';');
        }
    }
    // Finally, we clear the input, and reset the cursor position.
    currInput.clear();
    cursorPos = 0;
}


///
/// \brief Strip Whitespace (Leading & Trailing)
///
/// Modifies the passed string by removing all leading and trailing whitespace.
///
/// \param toTrim The string to trim the whitespace from.
///
void StripWhitespaceLT(std::string &toTrim)
{
    while (toTrim.length() > 0 && toTrim[0] == ' ')
    {
        toTrim.erase(0, 1);
    }
    while (toTrim.length() > 0 && toTrim[toTrim.length() - 1] == ' ')
    {
        toTrim.erase(toTrim.length() - 1, 1);
    }
}


///
/// \brief Input Backspace
///
/// Removes the last entered character, and moves the cursor to the left by one character.
///
void InputBackspace()
{
    if (cursorPos > 0)
    {
        cursorPos--;
        currInput.erase(cursorPos, 1);
    }
}


///
/// \brief Input Add Character
///
/// Adds the passed character to the \ref currInput string at the current \ref cursorPos.
///
/// \param c The character to add to the current input string.
/// \see   currInput | maxInputLength | cursorPos
///
void InputAddChar(char c)
{
    if (currInput.length() < maxInputLength)
    {
        if (currInput.length() == cursorPos)
        {
            currInput.push_back(c);
        }
        else
        {
            currInput.insert(cursorPos, 1, c);
        }
        cursorPos++;
    }
}


///
/// \brief Get Command
///
/// Attempts to find and return a pointer to the ConsoleCommand object representing the
/// command or alias passed to the function (or NULL if the command couldn't be found).
/// 
/// \param   cmdName String containing the command name or alias to find.
/// \returns A pointer to the console command, or NULL if no command could be found.
///
/// \see aliasList | cmdList | CommandAlias | ConsoleCommand
///
ConsoleCommand *GetCommand(std::string const& cmdName)
{
    std::list<CommandAlias*>::iterator aliasIt;
    for (aliasIt = aliasList.begin(); aliasIt != aliasList.end(); aliasIt++)
    {
        if (cmdName.compare((*aliasIt)->alias) == 0)
        {
            return GetCommand((*aliasIt)->name);
        }
    }

    std::list<ConsoleCommand*>::iterator cmdIt;
    for (cmdIt = cmdList.begin(); cmdIt != cmdList.end(); cmdIt++)
    {
        if (cmdName.compare((*cmdIt)->name) == 0)
        {
            return *cmdIt;
        }
    }
    return NULL;
}


///
/// \brief Suggest Command
///
/// Attempts to "auto-complete" the user's current input into the appropriate command.
/// If there is one (and only one) possible match, the command is auto-completed into
/// the currInput string.  If there are no matches, or more then one, nothing is modified.
///
/// \see aliasList | cmdList | currInput
///
void SuggestCommand()
{
    std::string toSuggest;                          // The name to suggest.
    unsigned char numMatches = 0;                   // Number of possible matches.
    size_t        matchLen,                         // Length of the current match.
                  currLen  = currInput.length();    // Length of the current input.
    bool matchFound = false;                        // Flag to set if a match is found.
    std::list<CommandAlias*>::iterator aliasIt;     // Iterator for the alias list.
    // So, looping through each entry in the alias list...
    for (aliasIt = aliasList.begin(); aliasIt != aliasList.end(); aliasIt++)
    {
        matchFound = true;                          // Assume we have a match initially.
        matchLen   = (*aliasIt)->alias.length();    // We store the length of the alias.
        // So, looping through the characters in both the input and command alias...
        for (size_t i = 0; (i < matchLen) && (i < currLen); i++)
        {
            if ((*aliasIt)->alias[i] != currInput[i])   // If any characters don't match...
            {
                matchFound = false;                         // Then toggle the match flag.
            }
        }
        if (matchFound)             // So, if we did find a match...
        {
            numMatches++;                   // Increment the match counter,
            toSuggest = (*aliasIt)->alias;  // and update the command to suggest.
        }
    }
    std::list<ConsoleCommand*>::iterator cmdIt;     // Iterator for the command list.
    // So, repeating the above steps, but this time looping through the command list...
    for (cmdIt = cmdList.begin(); cmdIt != cmdList.end(); cmdIt++)
    {
        matchFound = true;                          // Assume we have a match initially.
        matchLen   = (*cmdIt)->name.length();       // We store the length of the command.
        // So, looping through the characters in both the input and the command name...
        for (size_t i = 0; (i < matchLen) && (i < currLen); i++)
        {
            if ((*cmdIt)->name[i] != currInput[i])      // If any characters don't match...
            {
                matchFound = false;                         // Then toggle the match flag.
            }
        }
        if (matchFound)             // So, if we did find a match...
        {
            numMatches++;                   // Increment the match counter,
            toSuggest = (*cmdIt)->name;     // and update the command to suggest.
        }
    }
    if (numMatches == 1)        // Finally, if and only if we found one single match...
    {
        currInput = toSuggest;          // Replace the current input with the suggested
        cursorPos = toSuggest.length(); // command, and move the cursor to the end.
    }
}


///
/// \brief Get Constant Value
///
/// Used to convert possible argument values into their respective constant values.  For
/// example, if a user enters X_AXIS as a command's argument, this function will return
/// the value of the TC_X_AXIS constant.
///
/// \param   toConvert the string containing the constant to parse (case insensitive).
/// \returns The integer representing the passed constant, or -1 if it could not be parsed.
/// 
int GetConstantValue(const std::string &toConvert)
{
    std::string toConv(toConvert);
    // If the string length matches "?_AXIS" or "??_PLANE"...
    if (toConv.length() == 6 || toConv.length() == 8)
    {
        // We first convert the string to uppercase.
        for (size_t i = 0; i < toConv.length(); i++)
        {
            toConv[i] = std::toupper(toConv[i]);
        }
        // Now, we can see which constant (if any) the argument represents.
        // To save time, we do some extra comparisons (it's faster to compare lengths
        // and individual character values before comparing the entire string).
        if (toConv.length() == 6)
        {
            if (toConv[0] == 'X' && toConv == "X_AXIS") return TC_X_AXIS;
            if (toConv[0] == 'Y' && toConv == "Y_AXIS") return TC_Y_AXIS;
            if (toConv[0] == 'Z' && toConv == "Z_AXIS") return TC_Z_AXIS;
        }
        else
        {
            if (toConv[0] == 'X' && toConv == "XY_PLANE") return TC_XY_PLANE;
            if (toConv[0] == 'Y' && toConv == "YZ_PLANE") return TC_YZ_PLANE;
            if (toConv[0] == 'Z' && toConv == "ZX_PLANE") return TC_ZX_PLANE;
        }
    }
    // If the flow control gets here, then the string is not a constant, so return -1.
    return -1;
}
