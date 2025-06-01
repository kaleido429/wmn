#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// CRC 다항식: G = X^8 + X^2 + X + 1 = 100000111 (9비트)
#define GENERATOR 0x107

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

// CRC-8 계산 함수
unsigned char calculate_crc(unsigned short data) {
    unsigned int temp = (unsigned int)data << 8;
    unsigned int generator = GENERATOR << 15;
    
    for (int i = 0; i < 16; i++) {
        if (temp & 0x800000) {
            temp ^= generator;
        }
        temp <<= 1;
        generator >>= 1;
    }
    
    return (unsigned char)(temp >> 16);
}

// Hamming 인코딩 함수
void hamming_encode(int data[4], int codeword[7]) {
    for (int i = 0; i < 7; i++) {
        codeword[i] = 0;
        for (int j = 0; j < 4; j++) {
            codeword[i] ^= (G[j][i] * data[j]);
        }
    }
}

// Hamming 디코딩 함수 (에러 정정 포함)
int hamming_decode(int received[7], int decoded[4]) {
    // 신드롬 계산
    int syndrome[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            syndrome[i] ^= (H[i][j] * received[j]);
        }
    }
    
    int syndrome_value = syndrome[0] * 4 + syndrome[1] * 2 + syndrome[2];
    
    // 에러 정정
    int corrected[7];
    for (int i = 0; i < 7; i++) {
        corrected[i] = received[i];
    }
    
    if (syndrome_value != 0) {
        int error_pos = syndrome_value - 1;
        if (error_pos >= 0 && error_pos < 7) {
            corrected[error_pos] ^= 1;
        }
    }
    
    // 데이터 부분 추출
    for (int i = 0; i < 4; i++) {
        decoded[i] = corrected[i];
    }
    
    return syndrome_value; // 신드롬 값 반환
}

// 2-Layer 인코더
void two_layer_encoder() {
    printf("=== 2-Layer Channel Coding Encoder ===\n");
    char input[17];
    
    printf("Enter 16-bit data (binary): ");
    scanf("%16s", input);
    
    // 16비트 데이터를 정수로 변환
    unsigned short data = 0;
    for (int i = 0; i < 16; i++) {
        data = (data << 1) + (input[i] - '0');
    }
    
    // 1단계: CRC 인코딩 (16비트 -> 24비트)
    unsigned char crc = calculate_crc(data);
    
    printf("Step 1 - CRC encoding:\n");
    printf("Input: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
    printf("\n");
    printf("CRC result (24-bit): ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
    printf(" ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (crc >> i) & 1);
    }
    printf("\n");
    
    // 2단계: 24비트를 6개의 4비트 블록으로 나누어 각각 Hamming 인코딩
    printf("\nStep 2 - Hamming encoding (6 blocks of 4-bit -> 7-bit):\n");
    
    unsigned int crc_data = ((unsigned int)data << 8) | crc; // 24비트 데이터
    
    printf("Final encoded output (42-bit): ");
    
    for (int block = 0; block < 6; block++) {
        // 4비트씩 추출 (MSB부터)
        int shift = 20 - (block * 4);
        int data_4bit[4];
        
        for (int i = 0; i < 4; i++) {
            data_4bit[i] = (crc_data >> (shift - i)) & 1;
        }
        
        // Hamming 인코딩
        int codeword[7];
        hamming_encode(data_4bit, codeword);
        
        // 출력
        for (int i = 0; i < 7; i++) {
            printf("%d", codeword[i]);
        }
        if (block < 5) printf(" ");
    }
    printf("\n");
}

// 2-Layer 디코더
void two_layer_decoder() {
    printf("\n=== 2-Layer Channel Coding Decoder ===\n");
    printf("Enter 42-bit codeword (7 groups of 7-bit, space-separated): ");
    
    char block_inputs[6][8];
    for (int i = 0; i < 6; i++) {
        scanf("%7s", block_inputs[i]);
    }
    
    printf("\nStep 1 - Hamming decoding:\n");
    
    int decoded_blocks[6][4];
    int has_uncorrectable_error = 0;
    
    // 각 블록을 Hamming 디코딩
    for (int block = 0; block < 6; block++) {
        int received[7];
        for (int i = 0; i < 7; i++) {
            received[i] = block_inputs[block][i] - '0';
        }
        
        int syndrome = hamming_decode(received, decoded_blocks[block]);
        
        printf("Block %d: ", block + 1);
        for (int i = 0; i < 7; i++) {
            printf("%d", received[i]);
        }
        printf(" -> ");
        for (int i = 0; i < 4; i++) {
            printf("%d", decoded_blocks[block][i]);
        }
        printf(" (syndrome: %d)\n", syndrome);
    }
    
    // 24비트 데이터 재구성
    unsigned int reconstructed_24bit = 0;
    for (int block = 0; block < 6; block++) {
        for (int bit = 0; bit < 4; bit++) {
            reconstructed_24bit = (reconstructed_24bit << 1) | decoded_blocks[block][bit];
        }
    }
    
    printf("\nStep 2 - CRC checking:\n");
    printf("Reconstructed 24-bit: ");
    for (int i = 23; i >= 0; i--) {
        printf("%d", (reconstructed_24bit >> i) & 1);
        if (i == 8) printf(" ");
    }
    printf("\n");
    
    // 앞 16비트는 데이터, 뒤 8비트는 CRC
    unsigned short received_data = (reconstructed_24bit >> 8) & 0xFFFF;
    unsigned char received_crc = reconstructed_24bit & 0xFF;
    unsigned char calculated_crc = calculate_crc(received_data);
    
    printf("Received data: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (received_data >> i) & 1);
    }
    printf("\n");
    printf("Received CRC: %02X, Calculated CRC: %02X\n", received_crc, calculated_crc);
    
    if (calculated_crc == received_crc) {
        printf("\nResult: ");
        for (int i = 15; i >= 0; i--) {
            printf("%d", (received_data >> i) & 1);
        }
        printf(" (No error or corrected)\n");
    } else {
        printf("\nResult: ERROR!! (Uncorrectable error detected)\n");
    }
}

int main() {
    int choice;
    
    while (1) {
        printf("\n=== 2-Layer Channel Coding (CRC + Hamming) ===\n");
        printf("1. 2-Layer Encoder\n");
        printf("2. 2-Layer Decoder\n");
        printf("3. Exit\n");
        printf("Choose option (1-3): ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                two_layer_encoder();
                break;
            case 2:
                two_layer_decoder();
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