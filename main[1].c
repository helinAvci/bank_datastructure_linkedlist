#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct installment{
    char insid[30];
    short ispaid;
    char installmentdate[11];
    float amount;
    struct installment *nextins;
}installment;


typedef struct loan{
    char loanid[30];
    char type[30];
    float totalamount;
    int totalinstallmentnum;
    char processdate[11];
    struct loan *nextloan;
    installment *insptr;
}loan;

typedef struct customer{
    char name[20];
    char surname[30];
    int customerid;
    char customertype[20];
    struct customer *nextcust;
    double totaldebt;
    int totalLoans;
    loan *loanptr;
}customer;

void printCustomers(customer* customers) {
    printf("#####################################################\n");
    customer* curr = customers;
    while (curr != NULL) {
        printf("--------------------------------------------------\n");
        printf("%d - %s %s- type: %s-total Debt:%lf\n",curr->customerid, curr->name, curr->surname, curr->customertype,curr->totaldebt);
        curr = curr->nextcust;
    }
}

void readCustomers(struct customer **customers) {
    FILE *myFile;
    char name[20], surname[30], customertype[20];

    myFile = fopen("customers.txt", "r");

    if (myFile != NULL) {
        int customerid = 0;
        while (fscanf(myFile, "%s %s %s", name, surname, customertype) != EOF) {
            // Create a new customer node
            struct customer *newcust = (struct customer*) malloc(sizeof(struct customer));

            // Set customer information
            strcpy(newcust->name, name);
            strcpy(newcust->surname, surname);
            strcpy(newcust->customertype, customertype);
            newcust->totaldebt = 0.0;
            newcust->loanptr = NULL;
            newcust->nextcust = NULL;

            // Add the new customer node to the linked list
            if (*customers == NULL) {
                *customers = newcust;
            } else {
                struct customer *lastNode = *customers;
                while (lastNode->nextcust != NULL) {
                    lastNode = lastNode->nextcust;
                }
                lastNode->nextcust = newcust;
            }

            // Set the customer id
            customerid++;
            newcust->customerid = customerid;
            newcust->totalLoans=0;
        }
        fclose(myFile);
    } else {
        printf("Dosya acilamadi.\n");
    }
}

customer* getCustomerByNameSurname(customer* customers, char* name, char* surname) {
    customer* currCust = customers;
    while (currCust != NULL) {
        if (strcmp(currCust->name, name) == 0 && strcmp(currCust->surname, surname) == 0) {
            return currCust;
        }
        currCust = currCust->nextcust;
    }
    return NULL;
}



int compareDates(char* date1, char* date2) {// -1: date1<date2, 0: date1=date2, 1: date1>date2
    int day1, month1, year1, day2, month2, year2;
    sscanf(date1, "%d/%d/%d", &day1, &month1, &year1);
    sscanf(date2, "%d/%d/%d", &day2, &month2, &year2);

    if (year1 < year2)
        return -1;
    else if (year1 > year2)
        return 1;
    else
    if (month1 < month2)
        return -1;
    else if (month1 > month2)
        return 1;
    else
    if (day1 < day2)
        return -1;
    else if (day1 > day2)
        return 1;
    else
        return 0;

}

void updateID( loan* loans) {
    loan* current = loans;
    while (current->nextloan != NULL) {
        char id[30];
        int cID, lID;
        sscanf(current->loanid,"%dL%d",&cID,&lID);// parses the  loanid. the number before 'L' represents cID, and the number after 'L' represents lID.
        lID++;
        sprintf(current->nextloan->loanid,"%dL%d", cID, lID );// updates the loan ID of the next loan in the linked list. It formats the cID and incremented lID values into a string and stores it in
        current = current->nextloan;
    }

}


void addLoanToCustomer(customer* cust, loan* newloan) {
    char loanid[20];
    sprintf(loanid,"%d", cust->customerid );  // copy customerid to loanid
    strcat(loanid, "L1");  // concatenate "L1" to loanid

    // If customer has no loans, insert the new loan as the first one
    if (cust->loanptr == NULL) {
        strcpy(newloan->loanid, loanid);
        cust->loanptr = newloan;
        return;
    }

    // If the new loan is older than the first loan, insert it at the beginning
    if (compareDates(newloan->processdate, cust->loanptr->processdate) <= 0) {
        strcpy(newloan->loanid,loanid);
        newloan->nextloan = cust->loanptr;
        cust->loanptr = newloan;
        updateID(cust->loanptr);
        return;
    }

    // Find the position to insert the new loan
    loan* current = cust->loanptr;
    while (current->nextloan != NULL && compareDates(newloan->processdate, current->nextloan->processdate) > 0) {
        current = current->nextloan;
    }

    // Insert the new loan at the found position
    strcpy(newloan->loanid, loanid);
    newloan->nextloan = current->nextloan;
    current->nextloan = newloan;
    updateID(cust->loanptr);
}

void readLoans(customer* customers) {
    FILE* myFile;
    char name[20], surname[30], type[30], processdate[11];
    float totalamount;
    int totalinstallmentnum;
    char customerid[10];
    loan* newloan;

    myFile = fopen("loans.txt", "r");

    if (myFile != NULL) {
        while (fscanf(myFile, "%s %s %s %f %d %s", name, surname, type, &totalamount, &totalinstallmentnum, processdate) != EOF) {
            customer* cust=getCustomerByNameSurname(customers, name,surname);
            if(cust==NULL){
                printf("Reading loans failed. Customer %s %s not found.\n",name,surname );
                continue;
            }
            newloan = (loan*)malloc(sizeof(loan));
            char custID[4];  sprintf(custID, "%d", cust->customerid);
            strcpy(newloan->loanid,custID );
            strcat(newloan->loanid, "L");
            strcpy(newloan->type, type);
            newloan->totalamount = totalamount;
            newloan->totalinstallmentnum = totalinstallmentnum;
            strcpy(newloan->processdate, processdate);
            newloan->nextloan = NULL;
            newloan->insptr = NULL;
            addLoanToCustomer(cust, newloan);


        }
        fclose(myFile);

    }
    else {
        printf("file not found.\n");
    }
}


void printLoans(customer* cust) {

    printf("\n%d - %s %s - type: %s total debt: %lf\n",cust->customerid,cust->name, cust->surname, cust->customertype, cust->totaldebt);

    loan* loanptr = cust->loanptr;
    int loanCount = 0;
    while (loanptr != NULL) {
        printf(" %s: %s - %.2f - %s - %d\n", loanptr->loanid, loanptr->type, loanptr->totalamount, loanptr->processdate,loanptr->totalinstallmentnum);
        loanCount++;
        loanptr = loanptr->nextloan;
    }
    printf("--------------------------------------------------------------");
    if (loanCount == 0) {
        printf("No loans found for %s %s.\n", cust->name, cust->surname);
    }
}

void printAllLoans(customer* cust) {
    customer* curr=cust;
    while(curr!=NULL){
        printLoans(curr);
        curr=curr->nextcust;
    }
}
void createInstallments( customer* customers) {
    customer *temp = customers;
    while (temp != NULL){
        loan *temploan = temp->loanptr;
        while(temploan != NULL){
            //creat installment node
            int i = 1;
            while (i <= temploan->totalinstallmentnum) {
                struct installment *new = (struct installment*) malloc(sizeof(struct installment));

                new->nextins = NULL;
                new->ispaid = 0;
                new->amount = temploan->totalamount / temploan->totalinstallmentnum;
                strcpy(new->insid, temploan->loanid);
                strcat(new->insid, "I");
                char num[5];
                itoa(i ,num , 10);
                strcat(new->insid, num);
                i++;
                int day;
                int month;
                int year;
                sscanf(temploan->processdate , "%d/%d/%d" , &day,&month,&year);
                //clear until here
                if(month + i - 1 < 13){
                    month = (month + i -1);

                }
                else{
                    year = year + 1;
                    month = (month +i - 1)%12 + 1;

                }
                //convert date int to string;
                char day1[3];
                char month1[3];
                char year1[5];
                itoa(day,day1,10);
                itoa(month,month1,10);
                itoa(year,year1,10);
                strcpy(new->installmentdate ,day1);
                strcat(new->installmentdate,"/");
                strcat(new->installmentdate ,month1);
                strcat(new->installmentdate,"/");
                strcat(new->installmentdate ,year1);




                // add the installment to loan
                if(temploan->insptr == NULL){
                    temploan->insptr = new;
                }
                else{
                    installment *tempins = temploan->insptr;
                    while(tempins->nextins != NULL){
                        tempins =  tempins->nextins;
                    }
                    tempins->nextins = new;

                }
            }

            temploan = temploan->nextloan;

        }
        temp = temp->nextcust;

    }


}
void printInstallments( customer *customers) {
    customer *cust=customers;
    while(cust!=NULL) {
        //print customer
        printf("\n%d - %s %s - type: %s total debt: %lf\n", cust->customerid, cust->name, cust->surname,
               cust->customertype, cust->totaldebt);
        //print loan
        loan *loanptr = cust->loanptr;
        int loanCount = 0;
        while (loanptr != NULL) {
            printf(" %s: %s - %.2f - %s - %d\n", loanptr->loanid, loanptr->type, loanptr->totalamount,
                   loanptr->processdate, loanptr->totalinstallmentnum);
            // print installment
            installment *insptr = loanptr->insptr;
            if(insptr == NULL){
                puts("no ins\n");
            }
            while (insptr != NULL) {
                char payment[30];
                switch(insptr->ispaid){
                    case 0:strcpy(payment , "to be paid");break;
                    case 1:strcpy(payment , "paid");break;
                    case 2:strcpy(payment , "delayed");break;
                }
                printf("        %s -> %s -  %.2f -  %s\n" , insptr->insid, insptr->installmentdate,insptr->amount,payment);
                insptr = insptr->nextins;
            }
            loanCount++;
            loanptr = loanptr->nextloan;
        }
        printf("--------------------------------------------------------------");
        if (loanCount == 0) {
            printf("No loans found for %s %s.\n", cust->name, cust->surname);
        }

        cust = cust->nextcust;
    }
}



void readPayments(customer* customers) {
    FILE *payments = fopen("payments.txt", "r");
    char next[30];
    while (fgets(next, 30, payments)) {

        char *cust = strtok(next ,"L" );
        char *Loan = strtok(NULL , " ");
        char *ins = strtok(NULL, " ");

        // find customer
        customer *tempcust = customers;
        int Cust;

        sscanf(cust , "%d" , &Cust);
        while(Cust != tempcust->customerid){
            tempcust = tempcust->nextcust;
        }

        //find loan
        char ID[30];
        loan *temploan = tempcust->loanptr;
        char custid[30];
        itoa(tempcust->customerid , custid , 10);
        strcpy(ID , custid);
        strcat(ID , "L");
        strcat(ID , Loan);
        while(strcmp(ID , temploan->loanid) != 0){
            temploan = temploan->nextloan;

        }

        //find installment
        installment *tempins = temploan->insptr;
        char a = ins[0];
        int insId;
        if( a == 'A'){
            while(tempins != NULL){
                tempins->ispaid = 1;
                tempins = tempins->nextins;

            }
        }
        else {

            sscanf(ins , "%d" , &insId);
            int i = 1;
            while (i < insId && tempins != NULL ) {
                tempins = tempins->nextins;
                i++;
            }
            if(tempins != NULL) {
                tempins->ispaid = 1;
            }


        }


    }
    printInstallments(customers);
}

void findUnpaidInstallments( customer* customers){
    customer *tempcust = customers;
    char date[11];
    puts("please enter a date");
    scanf("%s" , &date);
    while(tempcust != NULL){
        int delayed = 0;
        loan *temploan = tempcust->loanptr;
        while(temploan != NULL) {
            installment *tempins = temploan->insptr;
            while (tempins != NULL) {
                if(compareDates(date ,tempins->installmentdate) == 1 && tempins->ispaid == 0){
                    tempins->ispaid = 2;
                    delayed += 1;
                    tempcust->totaldebt = tempcust->totaldebt + tempins->amount;
                }
                tempins = tempins->nextins;
            }
            temploan = temploan->nextloan;
        }
        if(delayed != 0) {
            printf("%s %s : Debt %.2f Number of Delayed Installment %d \n", tempcust->name, tempcust->surname,tempcust->totaldebt, delayed);
        }
        tempcust = tempcust->nextcust;
    }
}

void DeletePaidInstallments( customer* customers){
    customer *tempcust = customers;
    while(tempcust != NULL){
        loan *temploan = tempcust->loanptr;
        loan *front = NULL;
        loan *back = temploan->nextloan;
        while(temploan != NULL){
            bool payed;
            installment *tempins = temploan->insptr;
            int i;
            for(i = 0; i < temploan->totalinstallmentnum;i++){
                if(tempins->ispaid == 1){
                    tempins = tempins->nextins;
                    payed = 1;
                }
                else{
                    payed = 0;
                    break;
                }
            }
            if(payed == 1){
                if(front == NULL){
                    tempcust->loanptr = back;
                }
                else if(back == NULL){
                    front->nextloan = NULL;
                }
                else{
                    front->nextloan = back;
                }
            }
            front = temploan;
            temploan = temploan->nextloan;
            if(temploan == NULL){
                back =NULL;
            }
            else{
                back = temploan->nextloan;
            }


        }
        tempcust = tempcust->nextcust;
    }
    printInstallments(customers);
}



int main()
{
    int option = 1000;
    int i, n;
    customer *customers=NULL;


    while(option != 0){
        printf("\n###########################################################\n");
        printf("1) Read Customers\n");
        printf("2) Print Customers\n");
        printf("3) Read Loans\n");
        printf("4) Print Loans\n");
        printf("5) create installments\n");
        printf("6) Print installments\n");
        printf("7. read payments.\n");
        printf("8. find unpaid installments.\n");
        printf("9. delete completely paid installments.\n");
        printf("\n");
        printf("option : ");
        scanf("%d", &option);
        printf("\n###########################################################\n");
        switch(option){
            case 1:
                readCustomers(&customers);
                break;

            case 2:
                printCustomers(customers);
                break;
            case 3:
                readLoans(customers);

                break;
            case 4:
                printAllLoans(customers);
                break;
            case 5:
                createInstallments(customers);
                break;
            case 6:
                printInstallments(customers);
                break;
            case 7:
                readPayments(customers);

                break;
            case 8:
                findUnpaidInstallments(customers);
                break;
            case 9:
                DeletePaidInstallments(customers);
                break;

            default:
                break;
        }
    }
    return 0;
}

