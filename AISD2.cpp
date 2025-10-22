#include <iostream>
#include <algorithm>
#include <chrono>
#include <limits>
using namespace std;

const int MIN_MERGE = 32;

int calcMinRun(int n) {
    int r = 0;
    while (n >= MIN_MERGE) {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}

void insertionSort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void merge(int arr[], int left, int mid, int right) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    int* L = new int[len1];
    int* R = new int[len2];

    for (int i = 0; i < len1; i++) L[i] = arr[left + i];
    for (int i = 0; i < len2; i++) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    int gallopL = 0, gallopR = 0;

    while (i < len1 && j < len2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
            gallopL++;
            gallopR = 0;
        }
        else {
            arr[k++] = R[j++];
            gallopR++;
            gallopL = 0;
        }
        if (gallopL >= 7) {
            while (i < len1 && L[i] <= R[j]) arr[k++] = L[i++];
            gallopL = 0;
        }
        else if (gallopR >= 7) {
            while (j < len2 && R[j] < L[i]) arr[k++] = R[j++];
            gallopR = 0;
        }
    }

    while (i < len1) arr[k++] = L[i++];
    while (j < len2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void TimSort(int arr[], int n) {
    int minRun = calcMinRun(n);

    for (int i = 0; i < n; i += minRun) {
        insertionSort(arr, i, min(i + minRun - 1, n - 1));
    }
    for (int size = minRun; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min(left + 2 * size - 1, n - 1);
            if (mid < right)
                merge(arr, left, mid, right);
        }
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    int n;
    cout << "Введите количество элементов: " << endl;
    cin >> n;

    if (n <= 0) {
        cout << "Ошибка: количество элементов должно быть положительным числом!" << endl;
        return 1;
    }

    int* arr = new int[n];

    cout << "Введите " << n << " элементов массива (через пробел):" << endl;

    int count = 0;
    while (count < n) {
        if (cin >> arr[count]) {
            count++;
        }
        else {
            cout << "Ошибка: введено некорректное значение! Пожалуйста, введите число." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Исходный массив (" << n << " элементов):" << endl;
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << "\n";

    auto start = chrono::high_resolution_clock::now();
    TimSort(arr, n);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;

    cout << "Отсортированный массив (Timsort):" << endl;
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << "\n";

    cout << "Время сортировки: " << duration.count() << " секунд" << endl;

    delete[] arr;
    return 0;
}
