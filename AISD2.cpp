#include <iostream>
#include <algorithm>
#include <chrono>
#include <limits>
using namespace std;

const int MIN_MERGE = 32;
const int MAX_STACK_SIZE = 100;

struct Run {
    int start;
    int length;
};
class RunStack {
private:
    Run data[MAX_STACK_SIZE];
    int topIndex;

public:
    RunStack() : topIndex(-1) {}

    bool isEmpty() const {
        return topIndex == -1;
    }

    bool isFull() const {
        return topIndex == MAX_STACK_SIZE - 1;
    }

    void push(int start, int length) {
        if (!isFull()) {
            topIndex++;
            data[topIndex].start = start;
            data[topIndex].length = length;
        }
    }

    Run pop() {
        if (!isEmpty()) {
            Run result = data[topIndex];
            topIndex--;
            return result;
        }
        return Run{ 0, 0 };
    }

    Run peek(int indexFromTop = 0) const {
        if (topIndex >= indexFromTop) {
            return data[topIndex - indexFromTop];
        }
        return Run{ 0, 0 };
    }

    int size() const {
        return topIndex + 1;
    }

    void clear() {
        topIndex = -1;
    }
};

int calcMinRun(int n) {
    int r = 0;
    while (n >= MIN_MERGE) {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}


int binarySearch(int arr[], int key, int left, int right, bool findFirstGreater = false) {
    int low = left;
    int high = right;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == key) {
            return mid;
        }
        else if (arr[mid] < key) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }

    return findFirstGreater ? low : high;
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


int gallop(int arr[], int key, int start, int length, bool findRight) {
    int left = start;
    int right = start + length - 1;
    
    if (findRight) {
        int last = right;
        int offset = 1;

        while (last - offset >= left && arr[last - offset] >= key) {
            last -= offset;
            offset *= 2;
        }

        int gallopStart = max(left, last - offset);
        return binarySearch(arr, key, gallopStart, last, false) + 1 - start;
    } else {
        int last = left;
        int offset = 1;

        while (last + offset <= right && arr[last + offset] <= key) {
            last += offset;
            offset *= 2;
        }
        
        int gallopEnd = min(last + offset, right);
        return binarySearch(arr, key, last, gallopEnd, true) - start;
    }
}

void mergeWithGallop(int arr[], int left, int mid, int right) {
    int len1 = mid - left + 1;
    int len2 = right - mid;

    int* L = new int[len1];
    int* R = new int[len2];

    for (int i = 0; i < len1; i++) L[i] = arr[left + i];
    for (int i = 0; i < len2; i++) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    int gallopCount = 0;

    while (i < len1 && j < len2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
            gallopCount++;
        }
        else {
            arr[k++] = R[j++];
            gallopCount++;
        }

        if (gallopCount >= 7){
            gallopCount = 0;

            if (i < len1 && j < len2) {
                if (L[i] <= R[j]) {
                    int gallopEnd = gallop(L, R[j], i, len1 - i, false);
                    while (gallopEnd > 0) {
                        arr[k++] = L[i++];
                        gallopEnd--;
                    }
                }
                else {
                    int gallopEnd = gallop(R, L[i], j, len2 - j, true);
                    while (gallopEnd > 0) {
                        arr[k++] = R[j++];
                        gallopEnd--;
                    }
                }
            }
        }
    }

    while (i < len1) arr[k++] = L[i++];
    while (j < len2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

bool shouldMerge(const RunStack& stack) {
    if (stack.size() < 3) return false;

    Run X = stack.peek(2); 
    Run Y = stack.peek(1); 
    Run Z = stack.peek(0); 

    return (X.length <= Y.length + Z.length) || (Y.length <= Z.length);
}

void forceMerge(int arr[], RunStack& stack) {
    while (stack.size() > 1) {
        if (stack.size() > 2 && shouldMerge(stack)) {
            Run Z = stack.pop();
            Run Y = stack.pop();
            Run X = stack.pop();

            if (X.length <= Z.length) {
                mergeWithGallop(arr, X.start, X.start + X.length - 1, Y.start + Y.length - 1);
                stack.push(X.start, X.length + Y.length);
                stack.push(Z.start, Z.length);
            }
            else {
                mergeWithGallop(arr, Y.start, Y.start + Y.length - 1, Z.start + Z.length - 1);
                stack.push(X.start, X.length);
                stack.push(Y.start, Y.length + Z.length);
            }
        }
        else {
            break;
        }
    }
}

int findNaturalRun(int arr[], int start, int n) {
    if (start >= n - 1) return 1;

    int i = start;
    while (i < n - 1 && arr[i] <= arr[i + 1]) i++;

    int ascendingLength = i - start + 1;

    if (ascendingLength < MIN_MERGE && start < n - 1) {
        i = start;
        while (i < n - 1 && arr[i] >= arr[i + 1]) i++;
        int descendingLength = i - start + 1;

        if (descendingLength > 1) {
            int left = start;
            int right = start + descendingLength - 1;
            while (left < right) {
                swap(arr[left], arr[right]);
                left++;
                right--;
            }
            return descendingLength;
        }
    }

    return ascendingLength;
}

void TimSort(int arr[], int n) {
    if (n <= 1) return;

    int minRun = calcMinRun(n);
    RunStack stack;

    int i = 0;
    while (i < n) {
        int naturalRunLen = findNaturalRun(arr, i, n);
        int currentRunLen;

        if (naturalRunLen < minRun) {
            currentRunLen = min(minRun, n - i);
            insertionSort(arr, i, i + currentRunLen - 1);
        }
        else {
            currentRunLen = naturalRunLen;
        }

        stack.push(i, currentRunLen);

        forceMerge(arr, stack);
        i += currentRunLen;
    }

    while (stack.size() > 1) {
        Run Z = stack.pop();
        Run Y = stack.pop();
        mergeWithGallop(arr, Y.start, Y.start + Y.length - 1, Z.start + Z.length - 1);
        stack.push(Y.start, Y.length + Z.length);
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    int n;
    cout << "Введите количество элементов: ";
    cin >> n;

    if (n <= 0) {
        cout << "Ошибка: количество элементов должно быть положительным числом!" << endl;
        return 1;
    }

    int* arr = new int[n];

    cout << "Введите " << n << " элементов массива (через пробел):" << endl;

    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    cout << "Исходный массив:" << endl;
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    auto start = chrono::high_resolution_clock::now();
    TimSort(arr, n);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;

    cout << "Отсортированный массив:" << endl;
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    bool sorted = true;
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            sorted = false;
            break;
            
        }
    }
    cout << "Время сортировки: " << duration.count() << " секунд" << endl;

    delete[] arr;
    return 0;
}
