#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// CRC 다항식: G = X^8 + X^2 + X + 1
// 이진수로 100000111b -> HEX 0x107
#define GENERATOR_POLY 0x107 // 100000111b

// 16비트 데이터를 입력받아 CRC-8 값을 계산하고 인코딩된 출력을 생성
char* calculate_crc(unsigned short data) {
    unsigned char crc = 0; // CRC 레지스터 초기화
    static char encoded_output[30]; // 인코딩된 출력을 저장할 정적 버퍼

    // 16비트 데이터에 대해 CRC 계산
    for (int i = 15; i >= 0; i--) {
        if (((crc >> 7) & 0x01) ^ ((data >> i) & 0x01)) {
            crc = (crc << 1) ^ GENERATOR_POLY;
        } else {
            crc <<= 1;
        }
    }

    // 인코딩된 출력 생성
    sprintf(encoded_output, "Input data: ");
    for (int i = 15; i >= 0; i--) {
        char bit = (data >> i) & 1;
        sprintf(encoded_output + strlen(encoded_output), "%d", bit);
    }
    sprintf(encoded_output + strlen(encoded_output), "\nCRC-8 (FCS): ");
    for (int i = 7; i >= 0; i--) {
        char bit = (crc >> i) & 1;
        sprintf(encoded_output + strlen(encoded_output), "%d", bit);
    }
    sprintf(encoded_output + strlen(encoded_output), "\nEncoded (24-bit): ");
    for (int i = 15; i >= 0; i--) {
        char bit = (data >> i) & 1;
        sprintf(encoded_output + strlen(encoded_output), "%d", bit);
    }
    sprintf(encoded_output + strlen(encoded_output), " ");
    for (int i = 7; i >= 0; i--) {
        char bit = (crc >> i) & 1;
        sprintf(encoded_output + strlen(encoded_output), "%d", bit);
    }
    sprintf(encoded_output + strlen(encoded_output), "\n");

    return encoded_output; // 인코딩된 출력 반환
}

// CRC 인코더
void crc_encoder(char* input) {

    // 문자열을 정수로 변환
    unsigned short data = 0;
    for (int i = 0; i < 16; i++) {
        data = (data << 1) + (input[i] - '0');
    }
    // CRC 계산 및 인코딩된 출력 가져오기
    char* encoded_output = calculate_crc(data);

    // 인코딩된 출력 출력
    printf("%s", encoded_output);
}

// CRC 디코더
void crc_decoder(const char* received_input_str) { // char*를 인자로 받도록 수정
    printf("\n=== CRC-8 Decoder ===\n");

    // 24비트 전체를 codeword에 저장
    unsigned int codeword = 0;
    for (int i = 0; i < 24; i++) { // received_input_str 사용
        codeword = (codeword << 1) + (received_input_str[i] - '0');
    }

    unsigned char crc_check_value = 0;

    for (int i = 23; i >= 0; i--) { // 코드워드의 24비트에 대해 반복 (MSB부터)
        unsigned char current_bit = (codeword >> i) & 0x01; // 현재 비트 추출

        // 인코더의 calculate_crc와 동일한 로직 적용
        if ((((crc_check_value >> 7) & 0x01) ^ current_bit) != 0) {
            crc_check_value = (crc_check_value << 1) ^ GENERATOR_POLY;
        } else {
            crc_check_value = (crc_check_value << 1);
        }
    }
    // 수정된 CRC 검증 로직 끝

    // 최종 crc_check_value가 0이면 오류 없음
    if (crc_check_value == 0) {
        printf("Result: No error\n");
    } else {
        printf("Result: ERROR! Remainder: ");
        for (int i = 7; i >= 0; i--) {
            printf("%d", (crc_check_value >> i) & 1);
        }
        printf("\n");
    }
}

int main() {

    char input[25]; // 24비트 입력 + 종료 문자를 위해 25로 설정
    while (1) {
        printf("\n=== CRC-8 Encoder/Decoder ===\n");
        printf("입력 (16비트/24비트 이진수, -1 종료): ");
        scanf("%s", input);
        
        // -1 입력하면 종료
        if (strcmp(input, "-1") == 0) {
            break;
        }
        
        //input이 이진수 문자열인지 확인
        int valid = 1;
        for (int i = 0; i < strlen(input); i++) {
            if (input[i] != '0' && input[i] != '1') {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("오류: 이진수(0과 1)만 입력하세요!\n");
            continue;
        }
        // 입력 길이가 16비트 또는 24비트인지 확인
        if (strlen(input) != 16 && strlen(input) != 24) {
            printf("오류: 16비트 또는 24비트 이진수만 입력하세요!\n");
            continue;
        }
        //입력 길이 16bit면 crc_encoder 호출
        if (strlen(input) == 16) {
            crc_encoder(input);
        }
        
        //입력 길이 24bit면 crc_decoder 호출
        else if (strlen(input) == 24) {

            crc_decoder(input); 
        }
        else {
            printf("오류: 16비트 또는 24비트 이진수만 입력하세요!\n");
        }
    }

    return 0;
}