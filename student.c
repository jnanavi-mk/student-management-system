#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUBJECTS 5
#define TOTAL_FEES 50000

struct academics {
    float marks[SUBJECTS];
};

struct accounts {
    float fees_paid, fees_due;
};

struct student {
    int id;
    char name[50];
    int branch;
    int semester;
    struct academics acad;
    struct accounts acc;
};

// Function declarations
int login();
void addStudent();
void searchStudent();
void updateStudent();
void deleteStudent();
int checkDuplicate(int id);
char calculateGrade(float avg);
char* getBranchName(int branch);

// Input functions
void inputName(char name[]) {
    getchar();
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = 0;
}

int inputBranch() {
    int branch;
    printf("\nSelect Branch:\n1.ECE\n2.Civil\n3.CS\n4.Mech\n");
    scanf("%d", &branch);

    if(branch < 1 || branch > 4) {
        printf("Invalid branch!\n");
        return -1;
    }
    return branch;
}

int inputSemester() {
    int sem;
    printf("Enter Semester (1-8): ");
    scanf("%d", &sem);

    if(sem < 1 || sem > 8) {
        printf("Invalid semester!\n");
        return -1;
    }
    return sem;
}

void inputMarks(struct academics *acad) {
    printf("Enter marks for 5 subjects:\n");
    for(int i = 0; i < SUBJECTS; i++) {
        printf("Subject %d: ", i+1);
        scanf("%f", &acad->marks[i]);
    }
}

void inputFees(struct accounts *acc) {
    printf("Total Fees: %.2f\n", (float)TOTAL_FEES);
    printf("Enter Fees Paid: ");
    scanf("%f", &acc->fees_paid);

    acc->fees_due = TOTAL_FEES - acc->fees_paid;

    if(acc->fees_due < 0) {
        printf("Paid exceeds total! Setting due = 0\n");
        acc->fees_due = 0;
    }
}

// LOGIN
int login() {
    char user[20], pass[20];
    printf("\n===== LOGIN =====\n");
    printf("Username: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", pass);

    if(strcmp(user,"admin")==0 && strcmp(pass,"1234")==0) {
        printf("Login Successful!\n");
        return 1;
    } else {
        printf("Invalid Credentials!\n");
        return 0;
    }
}

// DUPLICATE CHECK
int checkDuplicate(int id) {
    FILE *fp = fopen("students.dat", "rb");
    struct student s;

    if(fp == NULL) return 0;

    while(fread(&s, sizeof(s), 1, fp)) {
        if(s.id == id) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// GRADE
char calculateGrade(float avg) {
    if(avg >= 90) return 'A';
    else if(avg >= 75) return 'B';
    else if(avg >= 60) return 'C';
    else return 'D';
}

// BRANCH NAME
char* getBranchName(int branch) {
    switch(branch) {
        case 1: return "ECE";
        case 2: return "Civil";
        case 3: return "CS";
        case 4: return "Mech";
        default: return "Unknown";
    }
}

// ADD STUDENT
void addStudent() {
    FILE *fp = fopen("students.dat", "ab");
    struct student s;

    printf("Enter ID: ");
    scanf("%d", &s.id);

    if(checkDuplicate(s.id)) {
        printf("ID already exists!\n");
        fclose(fp);
        return;
    }

    printf("Enter Name: ");
    inputName(s.name);

    s.branch = inputBranch();
    if(s.branch == -1) {
        fclose(fp);
        return;
    }

    s.semester = inputSemester();
    if(s.semester == -1) {
        fclose(fp);
        return;
    }

    inputMarks(&s.acad);
    inputFees(&s.acc);

    fwrite(&s, sizeof(s), 1, fp);
    fclose(fp);

    printf("Student Added Successfully!\n");
}

// SEARCH STUDENT
void searchStudent() {
    FILE *fp = fopen("students.dat", "rb");
    struct student s;
    int id, found = 0, ch;

    if(fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &id);

    while(fread(&s, sizeof(s), 1, fp)) {
        if(s.id == id) {
            found = 1;

            while(1) {
                printf("\n--- STUDENT MENU ---\n");
                printf("1. Personal Info\n2. Academics\n3. Accounts\n4. Exit\n");
                printf("Enter choice: ");
                scanf("%d", &ch);

                if(ch == 1) {
                    printf("\nID: %d\nName: %s\nBranch: %s\nSemester: %d\n",
                           s.id, s.name, getBranchName(s.branch), s.semester);
                }
                else if(ch == 2) {
                    float total = 0;

                    printf("\n--- Academics ---\n");
                    for(int i = 0; i < SUBJECTS; i++) {
                        printf("Subject %d: %.2f\n", i+1, s.acad.marks[i]);
                        total += s.acad.marks[i];
                    }

                    float avg = total / SUBJECTS;
                    char grade = calculateGrade(avg);

                    int pass = 1;
                    for(int i = 0; i < SUBJECTS; i++) {
                        if(s.acad.marks[i] < 40) {
                            pass = 0;
                            break;
                        }
                    }

                    printf("Total: %.2f\nAverage: %.2f\nGrade: %c\n", total, avg, grade);
                    printf("Result: %s\n", pass ? "PASS" : "FAIL");
                }
                else if(ch == 3) {
                    printf("\n--- Accounts ---\n");
                    printf("Total Fees : %.2f\n", (float)TOTAL_FEES);
                    printf("Fees Paid  : %.2f\n", s.acc.fees_paid);
                    printf("Fees Due   : %.2f\n", s.acc.fees_due);
                }
                else break;
            }
        }
    }

    if(!found) {
        printf("Student not found!\n");
    }

    fclose(fp);
}

// UPDATE STUDENT
void updateStudent() {
    FILE *fp = fopen("students.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    struct student s;
    int id, found = 0;

    if(fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while(fread(&s, sizeof(s), 1, fp)) {
        if(s.id == id) {
            found = 1;

            printf("Enter new Name: ");
            inputName(s.name);

            s.branch = inputBranch();
            if(s.branch == -1) continue;

            s.semester = inputSemester();
            if(s.semester == -1) continue;

            inputMarks(&s.acad);
            inputFees(&s.acc);
        }
        fwrite(&s, sizeof(s), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("students.dat");
    rename("temp.dat", "students.dat");

    if(found) printf("Updated Successfully!\n");
    else printf("Student not found!\n");
}

// DELETE STUDENT
void deleteStudent() {
    FILE *fp = fopen("students.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    struct student s;
    int id, found = 0;

    if(fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while(fread(&s, sizeof(s), 1, fp)) {
        if(s.id == id) {
            found = 1;
            continue;
        }
        fwrite(&s, sizeof(s), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("students.dat");
    rename("temp.dat", "students.dat");

    if(found) printf("Deleted Successfully!\n");
    else printf("Student not found!\n");
}

// MAIN
int main() {
    int choice;

    if(!login()) return 0;

    while(1) {
        printf("\n===== STUDENT MANAGEMENT SYSTEM =====\n");
        printf("1. Add Student\n2. Search Student\n3. Update Student\n4. Delete Student\n5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: addStudent(); break;
            case 2: searchStudent(); break;
            case 3: updateStudent(); break;
            case 4: deleteStudent(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}
