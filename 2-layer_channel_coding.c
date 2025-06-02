#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ... (compute_crc8_value, check_crc8_syndrome, encode_hamming_7_4, decode_hamming_7_4)
// ... (two_layer_encoder, two_layer_decoder)
// ... (from the previous response here)

// CRC-8 Definitions (from crc-8.c)
// CRC Polynomial: G = X^8 + X^2 + X + 1 (Binary 100000111b -> HEX 0x107)
#define GENERATOR_POLY 0x107

// (7,4) Hamming Code Definitions (from hamming-code.c)
// Generator Matrix G for (7,4) Hamming code
int G[4][7] = {
    {1, 0, 0, 0, 1, 0, 1},
    {0, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 0, 1, 1, 1},
    {0, 0, 0, 1, 0, 1, 1}
};

// Parity Check Matrix H for (7,4) Hamming code
int H[3][7] = {
    {1, 1, 1, 0, 1, 0, 0},
    {0, 1, 1, 1, 0, 1, 0},
    {1, 0, 1, 1, 0, 0, 1}
};

// Syndrome to error position mapping (0-indexed) for (7,4) Hamming code
int syndrome_table[8] = {
    0, 6, 5, 3, 4, 0, 1, 2
};

// CRC-8 계산 해서 24비트 인코딩된 출력 생성
unsigned char compute_crc8_value(unsigned short data_16bit) {
    unsigned char crc = 0; 
    for (int i = 15; i >= 0; i--) {
        if ((((crc >> 7) & 0x01) ^ ((data_16bit >> i) & 0x01)) != 0) {
            crc = (unsigned char)((crc << 1) ^ GENERATOR_POLY);
        } else {
            crc = (unsigned char)(crc << 1);
        }
    }
    return crc;
}

// CRC-8에서 받은 24비트 문자열의 CRC-8 신드롬을 검사 -> 에러 여부 반환
int check_crc8_syndrome(const char* received_24bit_str) {
    unsigned int codeword_val = 0;
    for (int i = 0; i < 24; i++) {
        codeword_val = (codeword_val << 1) | (received_24bit_str[i] - '0'); // 문자열을 정수로 변환
    }

    unsigned char crc_check_register = 0; 
    for (int i = 23; i >= 0; i--) {
        unsigned char current_codeword_bit = (codeword_val >> i) & 0x01; // 현재 비트 추출
        if ((((crc_check_register >> 7) & 0x01) ^ current_codeword_bit) != 0) {
            crc_check_register = (unsigned char)((crc_check_register << 1) ^ GENERATOR_POLY); // CRC-8 다항식 적용
        } else {
            crc_check_register = (unsigned char)(crc_check_register << 1);
        }
    }
    return (crc_check_register == 0) ? 0 : 1; // 나머지가 0이면 에러 없음, 1이면 에러 있음
}

// (7,4) Hamming 코드 인코딩 함수
// 입력은 4비트 문자열, 출력은 7비트 문자열
void encode_hamming_7_4(const char* input_4bit_str, char* output_7bit_str) {
    int data_bits[4];
    for (int i = 0; i < 4; i++) {
        data_bits[i] = input_4bit_str[i] - '0';
    }
    
    int codeword_bits[7] = {0}; 
    for (int i = 0; i < 7; i++) { 
        for (int j = 0; j < 4; j++) { 
            codeword_bits[i] ^= (G[j][i] * data_bits[j]); // G 행렬을 사용하여 데이터 비트와 곱셈
        }
    }
    
    for (int i = 0; i < 7; i++) {
        output_7bit_str[i] = codeword_bits[i] + '0';
    }
    output_7bit_str[7] = '\0'; 
}

// (7,4) Hamming 코드 디코딩 함수
// 입력은 7비트 문자열, 출력은 4비트 문자열
void decode_hamming_7_4(const char* input_7bit_str, char* output_4bit_str) {
    int received_bits[7];
    for (int i = 0; i < 7; i++) {
        received_bits[i] = input_7bit_str[i] - '0';
    }
    
    int syndrome_bits[3] = {0}; 
    for (int i = 0; i < 3; i++) { 
        for (int j = 0; j < 7; j++) { 
            syndrome_bits[i] ^= (H[i][j] * received_bits[j]);
        }
    }
    
    // 신드롬 값을 정수로 변환
    // syndrome_bits[0]은 4번째 비트, syndrome_bits[1]은 2번째 비트, syndrome_bits[2]는 1번째 비트
    int syndrome_value = syndrome_bits[0] * 4 + syndrome_bits[1] * 2 + syndrome_bits[2];
    
    int corrected_bits[7];
    for (int i = 0; i < 7; i++) {
        corrected_bits[i] = received_bits[i];
    }
    
    // 신드롬 값이 0이 아니면 에러가 발생한 것으로 간주
    if (syndrome_value != 0) { 
        int error_position = syndrome_table[syndrome_value]; 
        // 에러 위치가 0부터 6까지의 범위에 있는지 확인
        if (error_position >= 0 && error_position < 7) { 
             corrected_bits[error_position] ^= 1; 
        }
    }
    
    // 데이터 비트 추출 (첫 4비트)
    for (int i = 0; i < 4; i++) {
        output_4bit_str[i] = corrected_bits[i] + '0';
    }
    output_4bit_str[4] = '\0';
}

// 2-Layer CRC+Hamming 인코더
// 입력은 16비트 -> 24비트 CRC-8 인코딩 후 42비트 Hamming 코드로 인코딩
void two_layer_encoder(char* input_16bit_str) {
    unsigned short data_value = 0;
    for (int i = 0; i < 16; i++) {
        data_value = (data_value << 1) | (input_16bit_str[i] - '0');
    }

    unsigned char crc_value = compute_crc8_value(data_value);

    char crc_encoded_24bit_str[25];
    strncpy(crc_encoded_24bit_str, input_16bit_str, 16); 
    for (int i = 0; i < 8; i++) { 
        crc_encoded_24bit_str[16 + i] = ((crc_value >> (7 - i)) & 1) + '0';
    }
    crc_encoded_24bit_str[24] = '\0';

    char final_codeword_42bit[43];
    final_codeword_42bit[0] = '\0'; 

    for (int i = 0; i < 6; i++) { 
        char current_4bit_chunk_str[5];
        strncpy(current_4bit_chunk_str, crc_encoded_24bit_str + (i * 4), 4);
        current_4bit_chunk_str[4] = '\0';
        
        char hamming_encoded_7bit_chunk_str[8];
        encode_hamming_7_4(current_4bit_chunk_str, hamming_encoded_7bit_chunk_str);
        
        for(int j=0; j<7; ++j) {
            final_codeword_42bit[i*7 + j] = hamming_encoded_7bit_chunk_str[j];
        }
    }
    final_codeword_42bit[42] = '\0'; 

    printf("Output: ");
    for (int i = 0; i < 42; i++) {
        printf("%c", final_codeword_42bit[i]);
        if ((i + 1) % 7 == 0 && i < 41) { 
            printf(" ");
        }
    }
    printf("\n");
}

// 2-Layer CRC+Hamming 디코더
// 입력은 42비트 문자열 (공백 포함 가능) -> 24비트 Hamming 디코딩 후 CRC-8 검사
void two_layer_decoder(char* input_42bit_with_spaces_str) {
    char input_42bit_clean_str[43];
    int char_idx = 0;
    for(int i=0; input_42bit_with_spaces_str[i] != '\0' && char_idx < 42; ++i) {
        if (input_42bit_with_spaces_str[i] == '0' || input_42bit_with_spaces_str[i] == '1') {
            input_42bit_clean_str[char_idx++] = input_42bit_with_spaces_str[i];
        }
    }
    input_42bit_clean_str[char_idx] = '\0';

    if (strlen(input_42bit_clean_str) != 42) {
        printf("Error: Decoder input must be 42 binary bits (spaces are ignored).\n");
        return;
    }

    char hamming_decoded_24bit_str[25];
    hamming_decoded_24bit_str[0] = '\0'; 

    for (int i = 0; i < 6; i++) { 
        char current_7bit_chunk_str[8];
        strncpy(current_7bit_chunk_str, input_42bit_clean_str + (i * 7), 7);
        current_7bit_chunk_str[7] = '\0';
        
        char hamming_decoded_4bit_chunk_str[5];
        decode_hamming_7_4(current_7bit_chunk_str, hamming_decoded_4bit_chunk_str);
        
         for(int j=0; j<4; ++j) {
            hamming_decoded_24bit_str[i*4 + j] = hamming_decoded_4bit_chunk_str[j];
        }
    }
    hamming_decoded_24bit_str[24] = '\0'; 

    int crc_check_result = check_crc8_syndrome(hamming_decoded_24bit_str);

    if (crc_check_result == 0) { 
        printf("Output: ");
        for (int i = 0; i < 16; i++) { 
            printf("%c", hamming_decoded_24bit_str[i]);
        }
        printf("\n");
    } else { 
        printf("Output: ERROR!\n");
    }
}

int main() {
    char input_buffer[100]; //입력 버퍼(공백 고려해서 크게 설정)
    
    while (1) {
        printf("\n=== 2-Layer CRC+Hamming Encoder/Decoder ===\n");
        printf("Input (16-bit for encoder, 42-bit for decoder (can include spaces), -1 to exit): ");
        
       
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            // Handle EOF or read error
            printf("Error reading input or EOF reached. Exiting.\n");
            break;
        }
        
        input_buffer[strcspn(input_buffer, "\n")] = 0; // 개행 문자 제거

        // -1 입력하면 종료
        if (strcmp(input_buffer, "-1") == 0) {
            printf("Exiting program.\n");
            break;
        }
        
        // Validate input characters and count actual bits
        int is_valid_chars = 1;
        int bit_count = 0;
        size_t len = strlen(input_buffer); // Use size_t for strlen result
        for (size_t i = 0; i < len; i++) {
            if (input_buffer[i] == '0' || input_buffer[i] == '1') {
                bit_count++;
            } else if (input_buffer[i] != ' ') { // Allow spaces, but nothing else
                is_valid_chars = 0;
                break;
            }
        }
        
        if (!is_valid_chars) {
            printf("Error: Input must consist of binary digits ('0' or '1') and spaces only (for 42-bit input).\n");
            continue;
        }
        if (bit_count == 16) {
            char actual_16bit_input[17];
            int k=0;
            for(size_t i=0; i < len && k < 16; ++i) {
                if(input_buffer[i] == '0' || input_buffer[i] == '1') {
                    actual_16bit_input[k++] = input_buffer[i];
                }
            }
            actual_16bit_input[k] = '\0';

            if (strlen(actual_16bit_input) == 16) {
                 printf("Input data (16-bit): %s\n", actual_16bit_input);
                 two_layer_encoder(actual_16bit_input);
            } else {
                 printf("Error: Expected 16 binary digits for encoder after stripping spaces.\n");
            }
        } else if (bit_count == 42) {
            printf("Input codeword (42-bit as received): %s\n", input_buffer); 
            two_layer_decoder(input_buffer); 
        } else {
            printf("Error: Input must contain 16 binary digits (for encoder) or 42 binary digits (for decoder).\n");
            printf("Detected %d bits in input: '%s'\n", bit_count, input_buffer);

        }
    }
    return 0;
}