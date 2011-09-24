#include "util.h"

/************************************************************************
 * Function readRestOfLine() is used for buffer clearing. 
 * Please refer to "test_fgets.c" on Blackboard: 
 * "Course Documents"->"Function Examples"->"Input Validation Examples" 
 ************************************************************************/
void readRestOfLine()
{
  int c;
  
  /* Read until the end of the line or end-of-file. */
  while ((c = fgetc(stdin)) != '\n' && c != EOF)
    ;
  
  /* Clear the error and end-of-file flags. */
  clearerr(stdin);
}

/****************************************************************************
 * getString(): An interactive string input function.
 * This function prompts the user for a string using a custom prompt. A line
 * of text is accepted from the user using fgets() and stored in a temporary
 * string. When the function detects that the user has entered too much text,
 * an error message is given and the user is forced to reenter the input. The
 * function also clears the extra text (if any) with the readRestOfLine()
 * function.
 * When a valid string has been accepted, the unnecessary newline character
 * at the end of the string is overwritten. Finally, the temporary string is
 * copied to the string variable that is returned to the calling function.
 *
 * From: https://lms.rmit.edu.au/courses/1/COSC1284_1150/content/_3073503_1/
 * dir_StandardCLibraryFunctionExamples.zip/StandardCLibraryFunctionExamples/
 * InputValidation/getString-basic.c
 *
 * Modified to return RETURN_TO_MENU if a single carriage return is entered
 ****************************************************************************/
int getString(char* string, unsigned length, char* prompt)
{
  int finished = FALSE;
  char tempString[TEMP_STRING_LENGTH + 2];
  
  /* Continue to interact with the user until the input is valid. */
  do
  {
    /* Provide a custom prompt. */
    printf("%s", prompt);
    
    /* Accept input. "+2" is for the \n and \0 characters. */
    fgets(tempString, length + 2, stdin);
    
    if (tempString[0] == '\n')
    {
      /* user wants to return to main menu */
      return RETURN_TO_MENU;
    }
    /* A string that doesn't have a newline character is too long. */
    if (tempString[strlen(tempString) - 1] != '\n')
    {
      printf("Input was too long.\n");
      readRestOfLine();
    }
    else
    {
      finished = TRUE;
    }
    
  } while (finished == FALSE);
  
  /* Overwrite the \n character with \0. */
  tempString[strlen(tempString) - 1] = '\0';
  
  /* Make the result string available to calling function. */
  strcpy(string, tempString);
  
  return TRUE;
}

/****************************************************************************
 * getInteger(): An interactive integer input function.
 * This function prompts the user for an integer using a custom prompt. A line
 * of text is accepted from the user using fgets() and stored in a temporary
 * string. When the function detects that the user has entered too much text,
 * an error message is given and the user is forced to reenter the input. The
 * function also clears the extra text (if any) with the readRestOfLine()
 * function.
 * When a valid string has been accepted, the unnecessary newline character
 * at the end of the string is overwritten. The function then attempts to 
 * convert the string into an integer with strtol(). The function checks to
 * see if the input is numberic and within range.
 * Finally, the temporary integer is copied to the integer variable that is 
 * returned to the calling function.
 *
 * FROM: https://lms.rmit.edu.au/courses/1/COSC1284_1150/content/_3073503_1/
 * dir_StandardCLibraryFunctionExamples.zip/StandardCLibraryFunctionExamples/
 * InputValidation/getInteger-basic.c
 *
 * Modified to return RETURN_TO_MENU if a single carriage return is entered
 ****************************************************************************/
int getInteger(int* integer, unsigned length, char* prompt, int min, int max)
{
  int finished = FALSE;
  char tempString[TEMP_STRING_LENGTH + 2];
  int tempInteger = 0;
  char* endPtr;
  
  /* Continue to interact with the user until the input is valid. */
  do
  {
    /* Provide a custom prompt. */
    printf("%s", prompt);
    
    /* Accept input. "+2" is for the \n and \0 characters. */
    fgets(tempString, length + 2, stdin);
    
    if (tempString[0] == '\n')
    {
      /* user wants to return to main menu */
      return RETURN_TO_MENU;
    }
    /* A string that doesn't have a newline character is too long. */
    if (tempString[strlen(tempString) - 1] != '\n')
    {
      printf("Input was too long.\n");
      readRestOfLine();
    }
    else
    {
      /* Overwrite the \n character with \0. */
      tempString[strlen(tempString) - 1] = '\0';
      
      /* Convert string to an integer. */
      tempInteger = (int) strtol(tempString, &endPtr, 10);
      
      /* Validate integer result. */
      if (strcmp(endPtr, "") != 0)
      {
        printf("Input was not numeric.\n");
      }
      else if (tempInteger < min || tempInteger > max)
      {
        printf("Input was not within range %d - %d.\n", min, max);
      }
      else
      {
        finished = TRUE;
      }
    }
  } while (finished == FALSE);
  
  /* Make the result integer available to calling function. */
  *integer = tempInteger;
  
  return TRUE;
}
