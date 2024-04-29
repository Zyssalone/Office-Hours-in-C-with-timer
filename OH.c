// Libraries used in this code.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

/*Note: To change session duration please change the variable 'total_time' at line 208.*/

typedef struct nd // Structure used to store a node in a linked list.
{
    char s_name[20];
    int s_id, minutes;
    struct nd *next;
} s_info;

// Functions prototypes used in this code.
void menu();
int start_office_hours(s_info **front, s_info **rear, int hours, int minutes);
void serve(s_info **front, s_info **rear, int hour, int minute, int *duration);
void enqueue(s_info **front, s_info **rear, s_info *ToAdd);
void dequeue(s_info **front, s_info **rear, s_info **ToServe);
void new_student(s_info **front, s_info **rear);
void write_to_file(s_info *front, s_info *rear, FILE *file);
void LeaveQueue(s_info **front, s_info **rear, char *ToDelete);
void change_place(s_info *front, s_info *rear, char *Name1, char *Name2);
void swap(s_info *, s_info *);
int traverse_LL(s_info *front, s_info *rear);
void remove_newLine(char *arr);

// Main function.
int main()
{
    s_info *rear = NULL, *front = NULL, *ToServe;
    char choice, ToLeave[30], name1[20], name2[20];
    int process = 0, file_status = 0, duration;
    FILE *file = NULL;
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    system("cls");
    while (choice != 'f' && choice != 'F')
    {
        printf("\n\t\t\tIt's: %02d:%02d (Hour:Minute)\n", timeinfo->tm_hour, timeinfo->tm_min); // Time information.
        choice = 't';
        menu();
        do
        {
            printf("\n\t\t\t Enter your choice >>> ");
            scanf("%c", &choice);
            if (choice == '\n')
            {
                continue;
            }
            else if (isalpha(choice))
            {
                break;
            }
        } while (1); // Ensuring user input is correct.
        getchar();
        printf("\n");
        switch (choice)
        {
        case 'a': // Starting office hours.
        case 'A':
            system("cls"); // this command is used to clear the terminal, to get a better visual while handling the program.
            do
            {
                printf("\n\t\t\tOffice hours total duration\n\n\t\t\t(minutes) >>> ");
                scanf("%d", &duration);
            } while (duration > 60 || duration < 0);
            start_office_hours(&front, &rear, timeinfo->tm_hour, timeinfo->tm_min);
            while (duration > 0)
            {
                system("cls");
                traverse_LL(front, rear);
                serve(&front, &rear, timeinfo->tm_hour, timeinfo->tm_min, &duration);
                printf("\t\t<------------------------------------->\n");
                sleep(3);
                system("cls");
            }
            process = 1;
            getchar();
            break;
        case 'B': // Adding a new student to the queue.
        case 'b':
            system("cls");
            if (process == 0)
                printf("\t\tPlease start office hourse first!\n\n");
            else
                new_student(&front, &rear);
            break;
        case 'c': // Changing two students place in the queue.
        case 'C':
            system("cls");
            if (process == 0)
                printf("\t\tPlease start office hourse first!\n\n");
            else
            {
                printf("\t\tEnter the first student's name\n\n\t\tTo change place >>> ");
                fgets(name1, 20, stdin);
                remove_newLine(name1);
                printf("\n\t\tEnter the second student's name\n\n\t\tTo change place >>> ");
                fgets(name2, 20, stdin);
                remove_newLine(name2);
                change_place(front, rear, name1, name2);
            }
            break;
        case 'd': // Student leaving the queue.
        case 'D':
            system("cls");
            if (process == 0)
                printf("\t\tPlease start office hourse first!\n\n");
            else
            {
                printf("\t\tEnter student name\n\n\t\tTo leave the queue! >>> ");
                gets(ToLeave);
                LeaveQueue(&front, &rear, ToLeave);
            }
            break;
        case 'e': // Since we used "clear" command, we had to add the check queue.
        case 'E':
            system("cls");
            if (process == 0)
                printf("\t\tPlease start office hourse first!\n\n");
            else
            {
                traverse_LL(front, rear);
            }
            break;
        case 'f': // Ending office hours.
        case 'F':
            system("cls");
            printf("\n\t\tf. Office hours are over!\n\n\t\tEnding queue...\n\n\t\tWriting to file...\n\t\t\n");
            file = fopen("appointment.txt", "w");
            write_to_file(front, rear, file); // Writing to the file at the end.
            fclose(file);
            sleep(3);
            system("cls");
            break;
        default:
            system("cls");
            printf("\n\t\t   Please enter a correct choice!\n\n");
            break;
        }
    }
}

void menu() // Menu.
{
    printf("\n");
    printf("\t\t<------------------------------------->\n");
    printf("\t\t|                                     |\n");
    printf("\t\t| a. Start a new office hours session |\n");
    printf("\t\t|                                     |\n");
    printf("\t\t|b. A new student is joining the queue|\n");
    printf("\t\t|                                     |\n");
    printf("\t\t|c. Two students want to change places|\n");
    printf("\t\t|                                     |\n");
    printf("\t\t| d. A student wants to quit the queue|\n");
    printf("\t\t|                                     |\n");
    printf("\t\t|          e. Check the queue         |\n");
    printf("\t\t|                                     |\n");
    printf("\t\t|       f. Office hours are over!     |\n");
    printf("\t\t|                                     |\n");
    printf("\t\t<------------------------------------->\n");
}

int start_office_hours(s_info **front, s_info **rear, int hours, int minutes) // Start office hours function.
{
    FILE *infp;
    int i = 0, len;
    char buffer[50];
    s_info *new_node;
    infp = fopen("appointment.txt", "r");

    if (infp == NULL)
    {
        printf("\t\t\t\tFile doesn't exist!\n\t\t\tContinuing...\n\n");
    }
    else
    {
        while (!feof(infp)) // Reading from the file.
        {
            new_node = (s_info *)malloc(sizeof(s_info));
            fgets(new_node->s_name, 20, infp);
            remove_newLine(new_node->s_name);
            fscanf(infp, "%d", &new_node->s_id);
            fscanf(infp, "%d", &new_node->minutes);
            new_node->next = NULL;
            fgetc(infp);
            fgets(buffer, 50, infp);
            enqueue(front, rear, new_node); // Enqueuing.
        }
        printf("\n\t\t\tIt's: %02d:%02d (Hour:Minute)\n", hours, minutes);
        printf("\n\t\t\tOffice hours started!\n\n");
        printf("\n\t\t    %d students waiting in the queue.\n\n", traverse_LL(*front, *rear));
        printf("\t\t<------------------------------------->\n");
    }
    fclose(infp); // Closing the file.
}

void serve(s_info **front, s_info **rear, int hours, int minutes, int *duration) // Serving function.
{
    s_info *ToServe;
    int total_time = 10;
    dequeue(front, rear, &ToServe); // Dequeue at the start.
    if (ToServe == NULL)            // Queue empty.
        printf("\n\t\t\tQueue is empty.\n\n");
    else
    {
        printf("\n\t\t\tIt is: %02d:%02d\n", hours, minutes);
        printf("\n\t\t\t-%s- Come in please!\n", ToServe->s_name);
        printf("\n\t\t\tPROCESSING THE SESSION...\n");
        printf("\n\t\t\tTimer started: 5 minutes\n\n");
        printf("\t\t\tNB: Using %d seconds for testing\n\n", total_time);
        while (total_time > 0) // Timer for visual.
        {
            printf("\r\t\t\tTime remaining: %02d:%02d", total_time / 60, total_time % 60);
            fflush(stdout);
            sleep(1);
            total_time--;
        }
        printf("\n\n\t\t\tSESSION ENDED...\n");
        ToServe->minutes -= 5;
        (*duration) -= 5;
        if (ToServe->minutes <= 0)
        {
            printf("\n\t\t\t-%s- time to go!\n\n", ToServe->s_name); // No more time left.
        }
        else
        {
            printf("\n\t\t\tIt is: %02d:%02d\n", hours, minutes);
            printf("\n\t\t\t%d minutes left.\n\n\t\t -%s- is rejoining the queue\n\n", ToServe->minutes, ToServe->s_name);
            enqueue(front, rear, ToServe); // Returning to the queue, if student need more time.
        }
    }
}

void new_student(s_info **front, s_info **rear) // Adding a new student.
{
    s_info *new_node;
    new_node = (s_info *)malloc(sizeof(s_info));
    printf("\t\t\tEnter the student's name> ");
    gets(new_node->s_name);
    printf("\n\t\t\tEnter the student's ID> ");
    scanf("%d", &new_node->s_id);
    printf("\n\t\t\tEnter the number of minutes\n\n\t\t\t(10 - 20 - 30...)> ");
    scanf("%d", &new_node->minutes);
    getchar();
    enqueue(front, rear, new_node);
}

void enqueue(s_info **front, s_info **rear, s_info *ToAdd) // Enqueuing function.
{

    if (*front == NULL)
    {
        *front = ToAdd;
        *rear = ToAdd;
    }
    else
    {
        (*rear)->next = ToAdd;
        *rear = ToAdd;
        (*rear)->next = *front;
    }
}
void dequeue(s_info **front, s_info **rear, s_info **ToServe) // Dequeuing function.
{
    s_info *temp;
    temp = *front;
    if (*front == NULL)
        *ToServe = NULL;
    else if (*front == *rear)
    {
        *ToServe = *front;
        *front = NULL;
        *rear = NULL;
    }
    else
    {
        *ToServe = *front;
        *front = (*front)->next;
        (*rear)->next = *front;
    }
}

void write_to_file(s_info *front, s_info *rear, FILE *file) // Writing to the file at the end function.
{
    s_info *walker;
    walker = front;
    if (front != NULL)
    {
        do
        {
            fprintf(file, "%s\n%d\n%d\n", walker->s_name, walker->s_id, walker->minutes);
            fprintf(file, "-------------------------------------");
            walker = walker->next;
            if (walker != front)
                fprintf(file, "\n");
        } while (walker != front);
    }
}

void change_place(s_info *front, s_info *rear, char *Name1, char *Name2) // Changing places function.
{
    s_info *walker = front, *node1, *node2;
    int found1 = 0, found2 = 0;
    do
    {
        if (strcmp(walker->s_name, Name1) == 0)
        {
            found1 = 1;
            node1 = walker;
        }
        else if (strcmp(walker->s_name, Name2) == 0)
        {
            found2 = 1;
            node2 = walker;
        }
        walker = walker->next;
    } while (walker != front);

    if (found1 != 1)
        printf("\n\t\t\tStudent \"%s\" not found\n", Name1);
    else if (found2 != 1)
        printf("\n\t\t\tStudent \"%s\" not found\n", Name2);
    else
    {
        swap(node1, node2);
        printf("\n\t\tStudents have changed places.\n");
    }
}

void swap(s_info *A, s_info *B) // Swapping function for changing places function.
{
    s_info *temp = (s_info *)malloc(sizeof(s_info));

    strcpy(temp->s_name, A->s_name);
    temp->s_id = A->s_id;
    temp->minutes = A->minutes;

    strcpy(A->s_name, B->s_name);
    A->minutes = B->minutes;
    A->s_id = B->s_id;

    strcpy(B->s_name, temp->s_name);
    B->minutes = temp->minutes;
    B->s_id = temp->s_id;
}

void LeaveQueue(s_info **front, s_info **rear, char *ToDelete) // Leaving queue function.
{
    if (*front == NULL)
    {
        printf("\n\t\t\tQueue is empty. No one can leave!\n");
        return;
    }

    s_info *curr = *front, *prev = NULL;
    while (strcmp(curr->s_name, ToDelete) != 0) // Looking for the specified node.
    {
        if (curr->next == *front)
        {
            printf("\n\t\tStudent %s isn't present in the queue\n\n\t\tOr check spelling...\n\n", ToDelete);
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    if (*front == *rear && strcmp(curr->s_name, ToDelete) == 0) // One node in the queue.
    {
        *front = NULL;
        *rear = NULL;
        free(curr);
        printf("\n\t\t\tStudent %s\n\n\t\tHas left the queue!.\n\n", ToDelete);
        return;
    }

    if (curr == *front) // First node in the queue.
    {
        *front = (*front)->next;
        (*rear)->next = *front;
        free(curr);
        printf("\n\t\t\tStudent %s has left the queue!.\n\n", ToDelete);
        return;
    }

    if (curr == *rear) // Last node in the queue.
    {
        prev->next = *front;
        *rear = prev;
        free(curr);
        printf("\n\t\t\tStudent %s has left the queue!.\n\n", ToDelete);
        return;
    }

    prev->next = curr->next; // Any random node in the middle.
    free(curr);
    printf("\n\t\t\tStudent %s has left the queue!.\n\n", ToDelete);
}
int traverse_LL(s_info *front, s_info *rear) // Traversing the queue for starting office hours function.
{
    s_info *walker;
    int sum = 0;
    walker = front;
    if (front != NULL)
    {
        printf("\t\t\tStudents in queue are :\n");
        printf("\n\t\t-------------------------------------\n");
        do
        {
            printf("\n\t\t\tStudent: %s\n\t\t\t(ID: %d)\n\t\t\t(%d minutes left)\n", walker->s_name, walker->s_id, walker->minutes);
            printf("\n\t\t-------------------------------------\n");
            if (walker->next != NULL)
                walker = walker->next;
            sum++;
        } while (walker != front);
        return sum;
    }
    else
        printf("\n\t\t\tQueue is empty.\n\n");
}

void remove_newLine(char *arr) // Removing the "\n" at the end of the string to avoid misplacement of characters.
{
    int len;
    len = strlen(arr);
    if (len > 0 && arr[len - 1] == '\n')
        arr[len - 1] = '\0';
}
