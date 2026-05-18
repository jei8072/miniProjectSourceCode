#include <stdio.h>
#include <stdlib.h>

// structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void transfer(FILE *fPtr);

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            searchRecord(cfPtr);
            break;
        case 6:
            displayAll(cfPtr);
            break;
        case 7:
            transfer(cfPtr);
            break;
        default:
            printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

// menu
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\nMENU\n");
    printf("1 - Save accounts to text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Search account\n");
    printf("6 - Display all accounts\n");
    printf("7 - Transfer money\n");
    printf("9 - Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);
    return choice;
}

// create text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        printf("File could not be opened.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName,
                    client.firstName, client.balance);
        }
    }

    fclose(writePtr);
    printf("Data written to accounts.txt\n");
}

// update
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0};

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Current Balance: %.2f\n", client.balance);
    printf("Enter amount (+ deposit / - withdraw): ");
    scanf("%lf", &transaction);

    client.balance += transaction;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated Balance: %.2f\n", client.balance);
}

// add new
void newRecord(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance: ");
    scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance);

    client.acctNum = account;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created.\n");
}

// delete
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted.\n");
}

// search
void searchRecord(FILE *fPtr)
{
    unsigned int account;
    struct clientData client = {0};

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\n%-6d %-16s %-11s %.2f\n",
               client.acctNum, client.lastName,
               client.firstName, client.balance);
    }
}

// display all
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum, client.lastName,
                   client.firstName, client.balance);
        }
    }
}

// transfer
void transfer(FILE *fPtr)
{
    unsigned int from, to;
    double amount;

    struct clientData c1 = {0}, c2 = {0};

    printf("From account: ");
    scanf("%u", &from);

    printf("To account: ");
    scanf("%u", &to);

    printf("Amount: ");
    scanf("%lf", &amount);

    fseek(fPtr, (from - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c1, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (to - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&c2, sizeof(struct clientData), 1, fPtr);

    if (c1.acctNum == 0 || c2.acctNum == 0)
    {
        printf("Invalid account.\n");
        return;
    }

    if (c1.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    c1.balance -= amount;
    c2.balance += amount;

    fseek(fPtr, (from - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&c1, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (to - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&c2, sizeof(struct clientData), 1, fPtr);

    printf("Transfer successful.\n");
}