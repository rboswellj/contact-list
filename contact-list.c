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
void searchContact(Contact contacts[], int count);
void toLowerCase(char *str);
void trimNewline(char *str);
void printContact(Contact contact);
void clearInputBuffer();

// Validation prototypes
int validatePhoneNumber(const char *phone);
int validateEmail(const char *email);

// file operations
void saveContactsToFile(Contact contacts[], int count, const char *filename);
void loadContactsFromFile(Contact contacts[], int *count, const char *filename);
void createFileIfNotExists(const char *filename);

int main() {
    Contact contacts[100]; // Array to hold up to 100 contacts
    int count = 0; // Number of contacts currently in the list
    int choice;

    loadContactsFromFile(contacts, &count, "contacts.txt");

    do {
        printf("\nContact List Menu:\n");
        printf("1. Add New Contact\n");
        printf("2. Display All Contacts\n");
        printf("3. Search for a Contact by Name\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

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
                printf("Exiting the program.\n");
                saveContactsToFile(contacts, count, "contacts.txt");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

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
    trimNewline(contacts[*count].name);
    printf("Enter phone number: ");
    fgets(contacts[*count].phone, sizeof(contacts[*count].phone), stdin);
    trimNewline(contacts[*count].phone);
    printf("Enter email address: ");
    fgets(contacts[*count].email, sizeof(contacts[*count].email), stdin);
    trimNewline(contacts[*count].email);
    (*count)++;
    printf("Contact added successfully.\n");
}

// Function to display all contacts
void displayContacts(Contact contacts[], int count) {
    if (count == 0) {
        printf("No contacts to display.\n");
        return;
    }
    printf("\nContact List:\n");
    for (int i = 0; i < count; i++) {
        printContact(contacts[i]);
    }
}

// Function to search for a contact by name
void searchContact(Contact contacts[], int count) {
    char searchName[50];
    printf("Enter name to search: ");
    fgets(searchName, sizeof(searchName), stdin);
    trimNewline(searchName);
    toLowerCase(searchName);
    int found = 0;
    for (int i = 0; i < count; i++) {
        char lowerName[50];
        strcpy(lowerName, contacts[i].name);
        toLowerCase(lowerName);
        if (strcmp(lowerName, searchName) == 0) {
            printContact(contacts[i]);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Contact not found.\n");
    }
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
    printf("Name: %s\n", contact.name);
    printf("Phone: %s\n", contact.phone);
    printf("Email: %s\n", contact.email);
}

// Function to clear the input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Function to save contacts to a file
void saveContactsToFile(Contact contacts[], int count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
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
        createFileIfNotExists(filename);
        file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error opening file for reading.\n");
            return;
        }
    }
    char line[150];
    while (fgets(line, sizeof(line), file) && *count < 100) {
        trimNewline(line);
        char *token = strtok(line, ",");
        if (token != NULL) {
            strncpy(contacts[*count].name, token, sizeof(contacts[*count].name));
        }
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(contacts[*count].phone, token, sizeof(contacts[*count].phone));
        }
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(contacts[*count].email, token, sizeof(contacts[*count].email));
        }
        (*count)++;
    }
    fclose(file);
    printf("Contacts loaded from file successfully.\n");
}

// Function to create a file if it does not exist
void createFileIfNotExists(const char *filename) {
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

// Function to validate phone number (simple validation for digits and length)
int validatePhoneNumber(const char *phone) {
    int length = strlen(phone);
    if (length < 7 || length > 15) {
        return 0; // Invalid length
    }
    for (int i = 0; i < length; i++) {
        if (!isdigit(phone[i]) && phone[i] != '-' && phone[i] != ' ') {
            return 0; // Invalid character
        }
    }
    return 1; // Valid phone number
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

