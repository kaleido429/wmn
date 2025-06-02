#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// (7,4) Hamming code의 생성행렬 G
int G[4][7] = {
    {1, 0, 0, 0, 1, 0, 1},
    {0, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 0, 1, 1, 1},
    {0, 0, 0, 1, 0, 1, 1}
};

// 패리티 검사 행렬 H
int H[3][7] = {
    {1, 1, 1, 0, 1, 0, 0},
    {0, 1, 1, 1, 0, 1, 0},
    {1, 0, 1, 1, 0, 0, 1}
};

// 신드롬과 에러 위치 매핑 (수정된 버전)
int syndrome_table[8] = {
    0, // Syndrome 000: none (error_pos 0 for no error) [cite: 1]
    6, // Syndrome 001: r7 -> 0-indexed 6 [cite: 1]
    5, // Syndrome 010: r6 -> 0-indexed 5 [cite: 1]
    3, // Syndrome 011: r4 -> 0-indexed 3 [cite: 1]
    4, // Syndrome 100: r5 -> 0-indexed 4 [cite: 1]
    0, // Syndrome 101: r1 -> 0-indexed 0 [cite: 1]
    1, // Syndrome 110: r2 -> 0-indexed 1 [cite: 1]
    2  // Syndrome 111: r3 -> 0-indexed 2 [cite: 1]
};
// Hamming 인코더
void hamming_encoder(char* input) {
    printf("=== (7,4) Hamming Encoder ===\n");

    // 입력 문자열을 정수 배열로 변환
    int data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = input[i] - '0'; // '0'을 빼서 문자 -> 정수 변환
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
    for (int i = 0; i < 4; i++) {
        printf("%d", data[i]);
    }
    printf(" ");
    for (int i = 4; i < 7; i++) {
        printf("%d", codeword[i]);
    }
    printf("\n");
}

// Hamming 디코더 
void hamming_decoder(char* input) {
    printf("\n=== (7,4) Hamming Decoder ===\n");
    
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
    
    // 에러 정정
    int corrected[7];
    for (int i = 0; i < 7; i++) {
        corrected[i] = received[i];
    }
    
    if (syndrome_value != 0) {
        // 에러 위치 찾기 (신드롬 테이블 사용)
        int error_pos = syndrome_table[syndrome_value]; // 0-based 인덱스
        if (error_pos >= 0 && error_pos < 7) {
            corrected[error_pos] ^= 1; // 비트 플립
        }
    }
    
    // 데이터 부분 추출 (첫 4비트가 아니라 올바른 정보 비트를 추출)
    // 정보 비트는 G 행렬 구조에 따라 처음 4개 위치에 있음
    printf("Corrected data (4-bit): ");
    for (int i = 0; i < 4; i++) {
        printf("%d", corrected[i]);
    }
    printf(" (z: ");
    for (int i = 0; i < 3; i++) {
        printf("%d", syndrome[i]);
    }
    printf(")\n");
}

int main() {
    char input[8];
    
    while (1) {
        printf("\n=== (7,4) Hamming Code Encoder/Decoder ===\n");
        printf("입력 (4비트/7비트 이진수, -1 종료): ");
        scanf("%s", input);
        // -1 입력하면 종료
        if (strcmp(input, "-1") == 0) {
            break;
        }
        // 입력이 이진수 문자열인지 확인
        int valid = 1;
        for (int i = 0; i < strlen(input); i++) {
            if (input[i] != '0' && input[i] != '1') {
                valid = 0;
                break;
            }
        }
        // 입력 길이가 4비트 또는 7비트인지 확인
        if (!valid || (strlen(input) != 4 && strlen(input) != 7)) {
            printf("오류: 4비트 또는 7비트 이진수(0과 1)만 입력하세요!\n");
            continue;
        }
        // 4비트 입력이면 인코딩, 7비트 입력이면 디코딩
        if (strlen(input) == 4) {
            hamming_encoder(input);
        } else if (strlen(input) == 7) {
            hamming_decoder(input);
        }
    }
    return 0;
}