#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// CRC 다항식: G = X^8 + X^2 + X + 1 = 100000111 (9비트)
#define GENERATOR 0x107  // 100000111 in binary

// 16비트 데이터를 입력받아 CRC-8 값을 계산
unsigned char calculate_crc(unsigned short data) {
    unsigned int temp = (unsigned int)data << 8; // 16비트 데이터를 왼쪽으로 8비트 시프트
    unsigned int generator = GENERATOR << 15;    // 생성다항식을 맨 왼쪽으로 이동
    
    for (int i = 0; i < 16; i++) {
        if (temp & 0x800000) {  // 맨 왼쪽 비트가 1이면
            temp ^= generator;
        }
        temp <<= 1;
        generator >>= 1;
    }
    
    return (unsigned char)(temp >> 16);
}

// CRC 인코더
void crc_encoder() {
    printf("=== CRC-8 Encoder ===\n");
    char input[17];
    
    printf("Enter 16-bit data (binary): ");
    scanf("%16s", input);
    
    // 문자열을 정수로 변환
    unsigned short data = 0;
    for (int i = 0; i < 16; i++) {
        data = (data << 1) + (input[i] - '0');
    }
    
    // CRC 계산
    unsigned char crc = calculate_crc(data);
    
    // 결과 출력
    printf("Input data: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
    printf(" (0x%04X)\n", data);
    
    printf("CRC-8: ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (crc >> i) & 1);
    }
    printf(" (0x%02X)\n", crc);
    
    printf("Encoded (24-bit): ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
    printf(" ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (crc >> i) & 1);
    }
    printf(" (0x%04X%02X)\n", data, crc);
}

// CRC 디코더
void crc_decoder() {
    printf("\n=== CRC-8 Decoder ===\n");
    char input[25];
    
    printf("Enter 24-bit codeword (binary): ");
    scanf("%24s", input);
    
    // 앞 16비트는 데이터, 뒤 8비트는 CRC
    unsigned short received_data = 0;
    unsigned char received_crc = 0;
    
    // 데이터 부분 (앞 16비트)
    for (int i = 0; i < 16; i++) {
        received_data = (received_data << 1) + (input[i] - '0');
    }
    
    // CRC 부분 (뒤 8비트)
    for (int i = 16; i < 24; i++) {
        received_crc = (received_crc << 1) + (input[i] - '0');
    }
    
    // 받은 데이터로 CRC 재계산
    unsigned char calculated_crc = calculate_crc(received_data);
    
    printf("Received data: 0x%04X, Received CRC: 0x%02X\n", received_data, received_crc);
    printf("Calculated CRC: 0x%02X\n", calculated_crc);
    
    if (calculated_crc == received_crc) {
        printf("Result: No error\n");
    } else {
        printf("Result: ERROR!\n");
    }
}

int main() {
    int choice;
    
    while (1) {
        printf("\n=== CRC-8 Encoder/Decoder ===\n");
        printf("1. CRC Encoder\n");
        printf("2. CRC Decoder\n");
        printf("3. Exit\n");
        printf("Choose option (1-3): ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                crc_encoder();
                break;
            case 2:
                crc_decoder();
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