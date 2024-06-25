#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <unistd.h>
#include <sstream> 
#include <Windows.h> 
using namespace std;
struct Task {
    string name;
    int priority;
    string description;
    time_t deadline;
};

// Функції для порівняння завдань
bool comparePriority(const Task& a, const Task& b) {
    return a.priority < b.priority;
}
bool compareDeadline(const Task& a, const Task& b) {
    return a.deadline < b.deadline;
}
void printTask(const Task& task) {
    cout << "Назва: " << task.name << endl;
    cout << "Пріоритет: " << task.priority << endl;
    cout << "Опис: " << task.description << endl;
    cout << "Дата виконання: " << ctime(&task.deadline) << endl;
}
// Додаємо завдання 
void addTask(vector<Task>& tasks) {
    Task newTask;
    cout << "Введіть назву задачі: ";
    getline(cin, newTask.name);
    while (true) {
        cout << "Введіть пріоритет задачі (1-5): ";
        cin >> newTask.priority;
        if (newTask.priority >= 1 && newTask.priority <= 5) {
            break;
        }
        else {
            cout << "Error. Введіть значення від 1 до 5." << endl;
        }
    }
    cin.ignore();
    cout << "Введіть опис задачі: ";
    getline(cin, newTask.description);
    string deadlineStr;
    cout << "Введіть дату виконання задачі (DD MM YY): ";
    getline(cin, deadlineStr);

    struct tm tm = {};
    istringstream ss(deadlineStr);
    ss >> tm.tm_mday >> tm.tm_mon >> tm.tm_year;
    tm.tm_mon -= 1;
    tm.tm_year += 100;

    tm.tm_hour = 23;
    tm.tm_min = 59;
    tm.tm_sec = 59;

    newTask.deadline = mktime(&tm);

    tasks.push_back(newTask);
    sleep(0.5);
    cout << "\033[32mЗавдання збережено!\033[0m" << endl << endl;
    sleep(0.5);
}


void deleteTask(vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "Список справ порожній" << endl;
        return;
    }
    cout << "Введіть номер задачі, яку хочете видалити: ";
    int index;
    cin >> index;
    cin.ignore();
    if (index < 1 || index > tasks.size()) {
        cout << "\033[31mНеправильний номер задачі\033[0m" << endl;
        return;
    }
    tasks.erase(tasks.begin() + index - 1);
}

// Редагування
void editTask(vector<Task>& tasks) {
    if (tasks.empty()) {
        cout << "\033[35mСписок справ порожній\033[0m" << endl;
        return;
    }
    cout << "Введіть номер задачі для редагування: ";
    int index;
    cin >> index;
    cin.ignore();
    if (index < 1 || index > tasks.size()) {
        cout << "\033[31mНеправильний номер задачі\033[0m" << endl;
        return;
    }
    Task& task = tasks[index - 1];
    cout << "Введіть нове ім'я задачі : ";
    getline(cin, task.name);

    cout << "Введіть новий пріоритет задачі : ";
    string priorityStr;
    getline(cin, priorityStr);
    if (!priorityStr.empty()) {
        int newPriority = stoi(priorityStr);
        while (newPriority < 1 || newPriority > 5) {
            cout << "\033[31mError. Bведіть значення від 1 до 5: \033[0m";
            getline(cin, priorityStr);
            newPriority = stoi(priorityStr);
        }
        task.priority = newPriority;
    }

    cout << "Введіть новий опис задачі : ";
    getline(cin, task.description);

    cout << "Введіть нову дату виконання задачі (DD MM YY): ";
    string deadlineStr;
    getline(cin, deadlineStr);
    if (!deadlineStr.empty()) {
        struct tm tm = {};
        istringstream ss(deadlineStr);
        ss >> tm.tm_mday >> tm.tm_mon >> tm.tm_year;
        tm.tm_mon -= 1;
        tm.tm_year += 100;

        tm.tm_hour = 23;
        tm.tm_min = 59;
        tm.tm_sec = 59;

        task.deadline = mktime(&tm);
    }
}

void searchTasks(const vector<Task>& tasks) {
    cout << "Введіть текст для пошуку: ";
    string query;
    getline(cin, query);

    vector<Task> results;

    for (const Task& task : tasks) {
        if (task.name.find(query) != string::npos ||
            task.description.find(query) != string::npos) {

            results.push_back(task);
        }
    }
    if (results.empty()) {
        cout << "\033[35mЖодних завдань не знайдено\033[0m" << endl;
    }
    else {
        cout << "Результати пошуку:" << endl;
        for (const Task& task : results) {
            printTask(task);
            cout << endl;
        }
    }
}
void showTasksByDay(const vector<Task>& tasks) {
    time_t today = time(nullptr);
    tm* todayTm = localtime(&today);

    vector<Task> dailyTasks;

    for (const Task& task : tasks) {
        tm* taskTm = localtime(&task.deadline);

        if (taskTm->tm_year == todayTm->tm_year &&
            taskTm->tm_mon == todayTm->tm_mon &&
            taskTm->tm_mday == todayTm->tm_mday) {
            dailyTasks.push_back(task);
        }
    }

    if (dailyTasks.empty()) {
        cout << "\033[36mНа сьогодні немає завдань\033[0m" << endl;
    }
    else {
        cout << "Завдання на " << asctime(todayTm) << ":" << endl;
        for (const Task& task : dailyTasks) {
            printTask(task);
            cout << endl;
        }
    }
}

void showTasksByWeek(const vector<Task>& tasks) {
    time_t today = time(nullptr);
    tm* todayTm = localtime(&today);

    int currentWeek = (todayTm->tm_yday + 7 - todayTm->tm_wday) % 7;

    vector<Task> weeklyTasks;

    for (const Task& task : tasks) {
        tm* taskTm = localtime(&task.deadline);

        int taskWeek = (taskTm->tm_yday + 7 - taskTm->tm_wday) % 7;

        if (taskWeek == currentWeek) {
            weeklyTasks.push_back(task);
        }
    }

    if (weeklyTasks.empty()) {
        cout << "\033[36mНа цьому тижні немає завдань\033[0m" << endl;
    }
    else {
        cout << "Завдання на цьому тижні:" << endl;
        for (const Task& task : weeklyTasks) {
            printTask(task);
            cout << endl;
        }
    }
}

void showTasksByMonth(const vector<Task>& tasks) {
    time_t today = time(nullptr);
    tm* todayTm = localtime(&today);

    vector<Task> monthlyTasks;

    for (const Task& task : tasks) {
        tm* taskTm = localtime(&task.deadline);

        if (taskTm->tm_year == todayTm->tm_year &&
            taskTm->tm_mon == todayTm->tm_mon) {
            monthlyTasks.push_back(task);
        }
    }

    if (monthlyTasks.empty()) {
        cout << "\033[36mНа цьому місяці немає завдань\033[0m" << endl;
    }
    else {
        cout << "Завдання на цьому місяці:" << endl;
        for (const Task& task : monthlyTasks) {
            printTask(task);
            cout << endl;
        }
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    vector<Task> tasks;

    while (true) {
        cout << "\n\033[34m-----Меню-----\033[0m" << endl;
        cout << "1. Додати завдання" << endl << "2. Видалити завдання" << endl << "3. Редагувати завдання" << endl << "4. Пошук завдань" <<
            endl << "5. Вивести список справ на день" << endl << "6. Вивести список справ на тиждень" << endl << "7. Вивести список справ на місяць" <<
            endl << "8. Сортувати за пріоритетом" << endl << "9. Сортувати за часом виконання" << endl << "0. Вийти" << endl << "Ваш вибір: ";

        int choice;
        cin >> choice;
        cin.ignore();
        cout << endl;

        switch (choice) {
        case 1:
            sleep(1);
            addTask(tasks);
            break;
        case 2:
            sleep(1);
            deleteTask(tasks);
            break;
        case 3:
            sleep(1);
            editTask(tasks);
            break;
        case 4:
            sleep(1);
            searchTasks(tasks);
            break;
        case 5:
            sleep(1);
            showTasksByDay(tasks);
            break;
        case 6:
            sleep(1);
            showTasksByWeek(tasks);
            break;
        case 7:
            sleep(1);
            showTasksByMonth(tasks);
            break;
        case 8:
            sleep(1);
            sort(tasks.begin(), tasks.end(), comparePriority);
            cout << "\033[32mСписок справ успішно відсортовано за пріоритетом\033[0m" << endl;
            break;
        case 9:
            sleep(1);
            sort(tasks.begin(), tasks.end(), compareDeadline);
            cout << "\033[32mСписок справ успішно відсортовано за часом виконання\033[0m" << endl;
            break;
        case 0:
            cout << "\033[31mВихід із програми...\033[0m" << endl;
            return 0;
        default:
            cout << "\033[31mНеправильний вибір\033[0m" << endl;
        }
    }

    return 0;
}

