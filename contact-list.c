/*
Robert Johnson
CSE-130-50-4262
2/26/2026

Use an array of structures to create a contact list. 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define a structure for a contact
typedef struct {
    char name[50];
    char phone[15];
    char email[50];
} Contact;

// Function prototypes
void addContact(Contact contacts[], int *count);
void displayContacts(Contact contacts[], int count);
void alphabetizeContacts(Contact contacts[], int count);
void searchContact(Contact contacts[], int count);
int findMatchingContacts(Contact contacts[], int count, const char *query, int matchIndexes[], int maxMatches);
void toLowerCase(char *str);
void trimNewline(char *str);
void printContact(Contact contact);
void formatPhoneNumber(char *phone);
void deleteContact(Contact contacts[], int *count);
void confirmDelete(Contact contacts[], int index, int *count);
void displayRandomContact(Contact contacts[], int count);
void deleteAllContacts(Contact contacts[], int *count);
char* formatName(char *name);
void clearInputBuffer();

// Validation prototypes
int validatePhoneNumber(char *phone);
int validateEmail(const char *email);
void stripNonDigits(char *phone);

// file operations
void saveContactsToFile(Contact contacts[], int count, const char *filename);
void loadContactsFromFile(Contact contacts[], int *count, const char *filename);
void createFile(const char *filename);

int main() {
    Contact contacts[100]; // Array to hold up to 100 contacts
    int count = 0; // Number of contacts currently in the list
    int choice; // User selection for menu

    // Load contacts from file at the start of the program, 
    // creates file if it does not exist
    loadContactsFromFile(contacts, &count, "contacts.txt");

    do {
        printf("\nContact List Menu:\n\n");
        printf("1. Add New Contact\n");
        printf("2. Display All Contacts(Alphabetically)\n");
        printf("3. Search for a Contact by Name\n");
        printf("4. Delete a Contact\n");
        printf("5. Pick a random contact\n");
        printf("6. Delete All Contacts(Caution: This is permanent)\n");
        printf("7. Exit\n\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid choice. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                addContact(contacts, &count);
                break;
            case 2:
                displayContacts(contacts, count);
                break;
            case 3:
                searchContact(contacts, count);
                break;
            case 4:
                deleteContact(contacts, &count);
                break;
            case 5:
                displayRandomContact(contacts, count);
                break;
            case 6:
                deleteAllContacts(contacts, &count);
                break;
            case 7:
                printf("Exiting the program.\n");
                saveContactsToFile(contacts, count, "contacts.txt");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 7);

    return 0;
}

// Function to add a new contact
void addContact(Contact contacts[], int *count) {
    if (*count >= 100) {
        printf("Contact list is full. Cannot add more contacts.\n");
        return;
    }
    printf("Enter name: ");
    fgets(contacts[*count].name, sizeof(contacts[*count].name), stdin);
    if (strchr(contacts[*count].name, '\n') == NULL) {
        clearInputBuffer();
    }
    trimNewline(contacts[*count].name);
    formatName(contacts[*count].name);
    while(1){
        printf("Enter phone number(10 digits): ");
        fgets(contacts[*count].phone, sizeof(contacts[*count].phone), stdin);
        if (strchr(contacts[*count].phone, '\n') == NULL) {
            clearInputBuffer();
        }
        trimNewline(contacts[*count].phone);
        if (validatePhoneNumber(contacts[*count].phone)) {
            break;
        }
        printf("Invalid phone number format. Please try again.\n");
    } 
    formatPhoneNumber(contacts[*count].phone);
    while(1){
        printf("Enter email address: ");
        fgets(contacts[*count].email, sizeof(contacts[*count].email), stdin);
        if (strchr(contacts[*count].email, '\n') == NULL) {
            clearInputBuffer();
        }
        trimNewline(contacts[*count].email);
        if (validateEmail(contacts[*count].email)) {
            break;
        }
        printf("Invalid email format. Please try again.\n");
    }
    printf("Contact added successfully.\n");
    (*count)++;
}

// Function to alphabetize contacts by name using bubble sort
void alphabetizeContacts(Contact contacts[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(contacts[j].name, contacts[j + 1].name) > 0) {
                Contact temp = contacts[j];
                contacts[j] = contacts[j + 1];
                contacts[j + 1] = temp;
            }
        }
    }
}

// Function to display all contacts
void displayContacts(Contact contacts[], int count) {
    alphabetizeContacts(contacts, count);
    if (count == 0) {
        printf("No contacts to display.\n");
        return;
    }
    alphabetizeContacts(contacts, count);
    printf("\nContact List:\n");
    for (int i = 0; i < count; i++) {
        printContact(contacts[i]);
    }
}

// Function to search for a contact by name
void searchContact(Contact contacts[], int count) {
    char searchName[50];
    int matchIndexes[100];
    printf("Enter name to search: ");
    fgets(searchName, sizeof(searchName), stdin);
    if (strchr(searchName, '\n') == NULL) {
        clearInputBuffer();
    }
    trimNewline(searchName);
    int matchCount = findMatchingContacts(contacts, count, searchName, matchIndexes, 100);

    if (matchCount > 0) {
        printf("Found %d match(es):\n", matchCount);
        for (int i = 0; i < matchCount && i < 100; i++) {
            printContact(contacts[matchIndexes[i]]);
        }
    } else {
        printf("Contact not found.\n");
    }
}  

// Search for case-insensitive name contains matching
int findMatchingContacts(Contact contacts[], int count, const char *query, int matchIndexes[], int maxMatches) {
    char searchName[50];
    strncpy(searchName, query, sizeof(searchName) - 1);
    searchName[sizeof(searchName) - 1] = '\0';
    toLowerCase(searchName);

    int matchCount = 0;
    for (int i = 0; i < count; i++) {
        char lowerName[50];
        strncpy(lowerName, contacts[i].name, sizeof(lowerName) - 1);
        lowerName[sizeof(lowerName) - 1] = '\0';
        toLowerCase(lowerName);

        if (strstr(lowerName, searchName) != NULL) {
            if (matchCount < maxMatches) {
                matchIndexes[matchCount] = i;
            }
            matchCount++;
        }
    }
    return matchCount;
}

// Function to convert a string to lowercase
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Function to trim newline character from a string
void trimNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
} 

// Function to print a contact's details
void printContact(Contact contact) {
    printf("\n-------------------------\n");
    char displayName[sizeof(contact.name) + 2];
    char *space = strchr(contact.name, ' ');
    if (space != NULL) {
        int lastNameLength = (int)(space - contact.name);
        snprintf(displayName, sizeof(displayName), "%.*s, %s", lastNameLength, contact.name, space + 1);
        printf("Name: %s\n", displayName);
    } else {
        printf("Name: %s\n", contact.name);
    }
    printf("Phone: %s\n", contact.phone);
    printf("Email: %s\n", contact.email);
    printf("-------------------------\n");
}

// function to format phone number in (xxx) xxx-xxxx format
void formatPhoneNumber(char *phone) {
    char formatted[20];
    int j = 0;
    for (int i = 0; phone[i] != '\0'; i++) {
        if (isdigit(phone[i])) {
            if (j == 0) {
                formatted[j++] = '(';
            }
            formatted[j++] = phone[i];
            if (j == 4) {
                formatted[j++] = ')';
                formatted[j++] = ' ';
            } else if (j == 9) {
                formatted[j++] = '-';
            }
        }
    }
    formatted[j] = '\0';
    strncpy(phone, formatted, 20);
}

// Function to delete a contact by name
void deleteContact(Contact contacts[], int *count) {
    if (*count == 0) {
        printf("No contacts to delete.\n");
        return;
    }

    char searchName[50];
    int matchIndexes[100];
    int matchCount = 0;

    printf("Enter name of contact to delete: ");
    fgets(searchName, sizeof(searchName), stdin);
    if (strchr(searchName, '\n') == NULL) {
        clearInputBuffer();
    }
    trimNewline(searchName);
    matchCount = findMatchingContacts(contacts, *count, searchName, matchIndexes, 100);

    if (matchCount == 0) {
        printf("Contact not found. No deletion performed.\n");
        return;
    }

    if (matchCount == 1) {
        confirmDelete(contacts, matchIndexes[0], count);
        return;
    }

    printf("Multiple contacts found:\n");
    for (int i = 0; i < matchCount; i++) {
        printf("%d.\n", i + 1);
        printContact(contacts[matchIndexes[i]]);
    }

    printf("Please enter a more specific name.\n");

    deleteContact(contacts, count); // Recursive call to allow user to try again with a more specific search
}

// Function to confirm deletion of a contact
void confirmDelete(Contact contacts[], int index, int *count) {
    char confirmation[10];
    printContact(contacts[index]);
    printf("Are you sure you want to delete this contact? (yes/no): ");
    fgets(confirmation, sizeof(confirmation), stdin);
    if (strchr(confirmation, '\n') == NULL) {
        clearInputBuffer();
    }
    trimNewline(confirmation);
    toLowerCase(confirmation);
    if (strcmp(confirmation, "yes") == 0 || strcmp(confirmation, "y") == 0) {
        for (int i = index; i < *count - 1; i++) {
            contacts[i] = contacts[i + 1]; // Shift contacts to fill the gap
        }
        (*count)--;
        printf("Contact deleted successfully.\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}

// Function to clear the input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void deleteAllContacts(Contact contacts[], int *count) {
    char confirmation[10];
    printf("Are you sure you want to delete ALL contacts? (yes/no): ");
    fgets(confirmation, sizeof(confirmation), stdin);
    if (strchr(confirmation, '\n') == NULL) {
        clearInputBuffer();
    }
    trimNewline(confirmation);
    toLowerCase(confirmation);
    if (strcmp(confirmation, "yes") == 0 || strcmp(confirmation, "y") == 0) {
        // Clear the contacts array and reset count
        memset(contacts, 0, sizeof(Contact) * 100);
        *count = 0;
        printf("All contacts deleted successfully.\n");
    } else {
        printf("Deletion cancelled.\n");
    }
    saveContactsToFile(contacts, *count, "contacts.txt"); // Save the empty contact list to file
}   

void displayRandomContact(Contact contacts[], int count) {
    if (count == 0) {
        printf("No contacts to display.\n");
        return;
    }
    int randomIndex = rand() % count;
    printf("Random Contact:\n");
    printContact(contacts[randomIndex]);
}


// Function to validate phone number after stripping non-numeric symbols
int validatePhoneNumber(char *phone) {
    stripNonDigits(phone);
    int length = strlen(phone);
    if (length == 10) {
        return 1; // Valid phone number
    }
    return 0; // Invalid length
}

// Function to validate email address (simple validation for '@' and '.')
int validateEmail(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    if (at == NULL || dot == NULL || at > dot) {
        return 0; // Invalid email format
    }
    return 1; // Valid email address
}

// Function to keep only numeric characters in a phone number
void stripNonDigits(char *phone) {
    int write = 0;
    for (int read = 0; phone[read] != '\0'; read++) {
        if (isdigit((unsigned char)phone[read])) {
            phone[write++] = phone[read];
        }
    }
    phone[write] = '\0';
}

// Function to save contacts to a file
void saveContactsToFile(Contact contacts[], int count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return; // If file cannot be opened for writing, exit the function. Shouldn't happen, but also shouldn't crash the program if it exits. Just won't save
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%s,%s\n", contacts[i].name, contacts[i].phone, contacts[i].email);
    }
    fclose(file);
    printf("Contacts saved to file successfully.\n");
}

// Function to load contacts from a file
void loadContactsFromFile(Contact contacts[], int *count, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        createFile(filename);
        file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error creating file.\n");
            return; // If file creation fails, exit the function. Shouldn't happen, but also shouldn't crash the program if it exits.
        }
    }
    char line[150];
    while (fgets(line, sizeof(line), file) && *count < 100) {
        trimNewline(line);
        char *token = strtok(line, ",");
        if (token != NULL) {
            strncpy(contacts[*count].name, token, sizeof(contacts[*count].name) - 1);
            contacts[*count].name[sizeof(contacts[*count].name) - 1] = '\0';
        }
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(contacts[*count].phone, token, sizeof(contacts[*count].phone) - 1);
            contacts[*count].phone[sizeof(contacts[*count].phone) - 1] = '\0';
        }
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(contacts[*count].email, token, sizeof(contacts[*count].email) - 1);
            contacts[*count].email[sizeof(contacts[*count].email) - 1] = '\0';
        }

        (*count)++;
    }
    fclose(file);
    printf("Contacts loaded from file successfully.\n");
}

// Function to create a file if it does not exist
void createFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        file = fopen(filename, "w");
        if (file != NULL) {
            fclose(file);
            printf("File created successfully.\n");
        } else {
            printf("Error creating file.\n");
        }
    } else {
        fclose(file);
    }
}

char* formatName(char *name) {
    char formatted[50];
    char *lastName = strrchr(name, ' ');
    if (lastName != NULL) {
        lastName++; // Move past the space
        snprintf(formatted, sizeof(formatted), "%s %.*s", lastName, (int)(lastName - name - 1), name);
        return strncpy(name, formatted, 50);
    }
    return name; // If no space found, return the original name
}
