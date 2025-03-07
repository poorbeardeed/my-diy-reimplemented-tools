/**
 * my humble implementation of base32 algorithm.
 */

#ifndef BASE32_H
#define BASE32_H

#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>

#define BASE32_ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
#define BASE32_PADDING_SYMBOL '='

#define BASE32_PADDING_ENCOUNTED            (-1)
#define BASE32_INCORRECT_SYMBOL_ENCOUNTED   (-2)
#define BASE32_OUTPUT_LIMIT_REACHED         (-3)

#define BASE32_OK (0)


/**
 * @brief Calculates the length of a Base32-encoded string for a given data size.
 *
 * @param data_size Size of the input data in bytes.
 * @return The required length of the Base32-encoded string (including padding if necessary).
 */
size_t base32_encoded_str_len(size_t data_size){
    return ((data_size + 4) / 5) * 8 + 1;
}


/**
 * @brief Computes the maximum decoded data size from a Base32-encoded string length.
 *
 * @param str_len Length of the Base32-encoded string.
 * @return Maximum number of bytes the decoded data can occupy.
 */
size_t base32_decoded_data_size(size_t str_len){
    return (str_len / 8) * 5;
}


/**
 * @brief Encodes binary (any) data into a Base32-encoded string.
 * 
 * TODO: output limit
 * 
 * This function takes a binary input and encodes it into a Base32 string
 * using the standard Base32 alphabet (RFC 4648). The output string is
 * null-terminated.
 * 
 * @param data Pointer to the binary data to encode.
 * @param data_size Size of the binary data in bytes.
 * @param out Pointer to the output buffer for the encoded string.
 *            Must be large enough to hold the result, including
 *            padding and the null-terminator.
 */
void base32_encode(void* data, size_t data_size, char* out){
    const uint8_t* src = (const uint8_t*)data;
    size_t i, j; 
    uint8_t buffer[5];
    
    for (i = 0, j = 0; i < data_size; i += 5, j += 8) {

        size_t chank_size = (data_size - i < 5) ? (data_size - i) : 5;

        memset(buffer, 0, 5);
        memcpy(buffer, src + i, chank_size);
        
        // Encode 5-byte block into 8 Base32 characters
        switch(chank_size){ // falls through!
            case 5: out[j+7] = BASE32_ALPHABET[buffer[4] & 0x1F];
            case 4: out[j+6] = BASE32_ALPHABET[((buffer[3] & 0x03) << 3) | (buffer[4] >> 5)];
                    out[j+5] = BASE32_ALPHABET[(buffer[3] & 0x7C) >> 2];
            case 3: out[j+4] = BASE32_ALPHABET[((buffer[2] & 0x0F) << 1) | (buffer[3] >> 7)];
            case 2: out[j+3] = BASE32_ALPHABET[((buffer[1] & 0x01) << 4) | (buffer[2] >> 4)];
            case 1: out[j+2] = BASE32_ALPHABET[(buffer[1] & 0x3E) >> 1];
                    out[j+1] = BASE32_ALPHABET[((buffer[0] & 0x07) << 2) | (buffer[1] >> 6)];
                    out[j]   = BASE32_ALPHABET[buffer[0] >> 3];
            // case 0: // redundant
        }

        
        switch(chank_size){ // falls through!
            // case 0: out[j]   = BASE32_PADDING_SYMBOL; // redundant
            //         out[j+1] = BASE32_PADDING_SYMBOL; 
            case 1: out[j+2] = BASE32_PADDING_SYMBOL; 
                    out[j+3] = BASE32_PADDING_SYMBOL; 
            case 2: out[j+4] = BASE32_PADDING_SYMBOL; 
            case 3: out[j+5] = BASE32_PADDING_SYMBOL; 
                    out[j+6] = BASE32_PADDING_SYMBOL; 
            case 4: out[j+7] = BASE32_PADDING_SYMBOL;  
            // case 5: // redundant
        }
    }
    out[j] = '\0'; // Null-terminate output string
}


/**
 * @brief Converts a single Base32 character to its numerical value.
 *
 * @param ch The Base32 character to convert.
 * @return The numerical value of the character (0-31), 
 *         BASE32_PADDING_ENCOUNTED for padding (BASE32_PADDING_SYMBOL),
 *         or BASE32_INCORRECT_SYMBOL for an invalid character.
 */
int base32_char_to_value(char ch){
    if(ch >= 'A' && ch <= 'Z')      return ch - 'A';
    if(ch >= '2' && ch <= '7')      return 26 + ch - '2';
    if(ch == BASE32_PADDING_SYMBOL) return BASE32_PADDING_ENCOUNTED;
    return BASE32_INCORRECT_SYMBOL_ENCOUNTED;
}


/**
 * @brief Decodes a Base32-encoded string into binary data.
 *
 * @param str Pointer to the Base32-encoded null-terminated string.
 * @param out Pointer to the output buffer where decoded data will be stored.
 * @param out_limit Maximum number of bytes that can be written to the output buffer.
 * @return negative value on error.
 */
int base32_decode(const char* str, void* out, size_t out_limit){

    uint8_t* out_uint8 = (uint8_t*)out;
    uint8_t buffer[8];
    
    for(size_t i = 0, j = 0; j < out_limit; j+=5){

        memset(buffer, 0, sizeof(buffer));      
        bool isPaddingReached = false;
        
        for(int k = 0; k < 8 && str[i] != '\0'; k++){
            buffer[k] = 0;
            // handle special cases
            switch(int value = base32_char_to_value(str[i++])){ 
                case BASE32_INCORRECT_SYMBOL_ENCOUNTED  : return BASE32_INCORRECT_SYMBOL_ENCOUNTED;
                case BASE32_PADDING_ENCOUNTED           : isPaddingReached = true; continue;
                default:
                    // cannot have values after padding symbol, failed to decode;
                    if(isPaddingReached) return BASE32_INCORRECT_SYMBOL_ENCOUNTED; 

                    // copy value to the buffer
                    buffer[k] = (uint8_t)value;
            }
        }
        
        switch((out_limit - j) < 5 ? (out_limit - j) : 5){ // falls through
            case 5: out_uint8[j+4] = (buffer[6] << 5) | (buffer[7]);
            case 4: out_uint8[j+3] = (buffer[4] << 7) | (buffer[5] << 2) | (buffer[6] >> 3);
            case 3: out_uint8[j+2] = (buffer[3] << 4) | (buffer[4] >> 1);
            case 2: out_uint8[j+1] = (buffer[1] << 6) | (buffer[2] << 1) | (buffer[3] >> 4);
            case 1: out_uint8[j  ] = (buffer[0] << 3) | (buffer[1] >> 2);
        }
    }
    return BASE32_OK;
}


// ------------------------------------------------------------------------


void test_base32_encoding() {
    const char* test_cases[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar", "Hello, World!", "asdqwd", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======", "mmmmm", "AZXW6YTBOI======1"
        ,"sizeof() returns the size in bytes of its operand, but its meaning depends on what the operand is"
    };

    char decoded[1024];
    char encoded[1024];
    
    for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        memset(encoded, '~', sizeof(encoded));
        memset(decoded, '~', sizeof(decoded));

        base32_encode((void*)test_cases[i], strlen(test_cases[i]), encoded);

        if(int base32_result = base32_decode(encoded, decoded, sizeof(decoded)) != BASE32_OK){
            fprintf(stderr, "decoding failed with code: %d\n", base32_result);
        }

        printf("Test %zu: %s\n", i + 1, strcmp(test_cases[i], decoded)==0 ? "PASS" : "FAIL");
        
        if(strcmp(test_cases[i], decoded)!=0){

            printf("    >> input    :%s\n", test_cases[i]);
            printf("    >> decoded  :%s\n", decoded);
            //printf("    >> encoded  :%s\n", encoded);
        }
    }
}

#endif//!BASE32_H