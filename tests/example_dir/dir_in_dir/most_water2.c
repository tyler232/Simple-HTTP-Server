#include <stdio.h>

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int maxArea(int* height, int heightSize) {
    int max_area = 0;
    int left = 0;
    int right = heightSize - 1;

    while (left < right) {
        int current_area = (right - left) * min(height[left], height[right]);
        max_area = max(max_area, current_area);

        if (height[left] < height[right])
            left++;
        else
            right--;
    }

    return max_area;
}
