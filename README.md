crc_encoder: Convert 'dataword' to 'codeword' using CRC.
crc_decoder: While restoring 'codeword' to 'dataword', detects errors.

(0) How to compile
g++ -o crc_encoder crc_encoder.cc
g++ -o crc_decoder crc_decoder.cc

(1) CRC encoding
./crc_encoder input_file output_file generator dataword_size 
* generator : 0000 ~ 1111 (binary)
* dataword_size : 4 or 8
ex) ./crc_encoder input.txt output.txt 1001 4

(2) CRC decoding
./crc_decoder input_file output_file result_file generator dataword_size
ex) ./crc_decoder input.txt output.txt result.txt 1001 4

(3) Result file example
90 21
This means that 90 codewords were transmitted, of which 21 errors were detected.
