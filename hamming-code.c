#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// (7,4) Hamming code의 생성행렬 G
int G[4][7] = {
    {1, 0, 0, 0, 1, 0, 1},
    {0, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 1, 1}
};

// 패리티 검사 행렬 H
int H[3][7] = {
    {1, 1, 1, 0, 1, 0, 0},
    {0, 1, 1, 1, 0, 1, 0},
    {1, 0, 1, 1, 0, 0, 1}
};

// 신드롬과 에러 위치 매핑
int syndrome_table[8] = {0, 6, 5, 4, 3, 2, 1, 0}; // 인덱스는 신드롬, 값은 에러 비트 위치 (1-based)

// Hamming 인코더
void hamming_encoder() {
    printf("=== (7,4) Hamming Encoder ===\n");
    char input[5];
    
    printf("Enter 4-bit data (binary): ");
    scanf("%4s", input);
    
    // 입력 데이터를 배열로 변환
    int data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = input[i] - '0';
    }
    
    // 코드워드 계산: t = G^T * s
    int codeword[7] = {0};
    
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            codeword[i] ^= (G[j][i] * data[j]);
        }
    }
    
    // 결과 출력
    printf("Input data: ");
    for (int i = 0; i < 4; i++) {
        printf("%d", data[i]);
    }
    printf("\n");
    
    printf("Encoded codeword (7-bit): ");
    for (int i = 0; i < 7; i++) {
        printf("%d", codeword[i]);
    }
    printf(" (");
    for (int i = 0; i < 4; i++) {
        printf("%d", data[i]);
    }
    printf(" ");
    for (int i = 4; i < 7; i++) {
        printf("%d", codeword[i]);
    }
    printf(")\n");
}

// Hamming 디코더
void hamming_decoder() {
    printf("\n=== (7,4) Hamming Decoder ===\n");
    char input[8];
    
    printf("Enter 7-bit codeword (binary): ");
    scanf("%7s", input);
    
    // 받은 코드워드를 배열로 변환
    int received[7];
    for (int i = 0; i < 7; i++) {
        received[i] = input[i] - '0';
    }
    
    // 신드롬 계산: z = H * r
    int syndrome[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            syndrome[i] ^= (H[i][j] * received[j]);
        }
    }
    
    // 신드롬을 정수로 변환
    int syndrome_value = syndrome[0] * 4 + syndrome[1] * 2 + syndrome[2];
    
    printf("Received codeword: ");
    for (int i = 0; i < 7; i++) {
        printf("%d", received[i]);
    }
    printf("\n");
    
    printf("Syndrome z = %d%d%d (%d)\n", syndrome[0], syndrome[1], syndrome[2], syndrome_value);
    
    // 에러 정정
    int corrected[7];
    for (int i = 0; i < 7; i++) {
        corrected[i] = received[i];
    }
    
    if (syndrome_value != 0) {
        // 에러 위치 찾기 (신드롬 값이 곧 에러 비트 위치)
        int error_pos = syndrome_value - 1; // 0-based 인덱스
        if (error_pos >= 0 && error_pos < 7) {
            corrected[error_pos] ^= 1; // 비트 플립
            printf("Error detected at position %d, corrected\n", error_pos + 1);
        }
    } else {
        printf("No error detected\n");
    }
    
    // 데이터 부분 추출 (첫 4비트)
    printf("Corrected data (4-bit): ");
    for (int i = 0; i < 4; i++) {
        printf("%d", corrected[i]);
    }
    printf("\n");
}

int main() {
    int choice;
    
    while (1) {
        printf("\n=== (7,4) Hamming Code Encoder/Decoder ===\n");
        printf("1. Hamming Encoder\n");
        printf("2. Hamming Decoder\n");
        printf("3. Exit\n");
        printf("Choose option (1-3): ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                hamming_encoder();
                break;
            case 2:
                hamming_decoder();
                break;
            case 3:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid option!\n");
        }
    }
    
    return 0;
}