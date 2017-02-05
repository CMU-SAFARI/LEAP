/*
 * vector_filterMain.c
 *
 *  Created on: Nov 12, 2013
 *      Author: hxin
 */

//#ifndef BOOST_PP_IS_ITERATING
//#include "print.h"
#include <string>
#include <sys/times.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "SIMD_ED.h"
#include "string_buffer.h"
#include "needleman_wunsch.h"
#include "nw_cmdline.h"
#include "parasail.h"

#define BATCH_RUN 1000000
#ifndef _MAX_LENGTH_
#define _MAX_LENGTH_ 256
#endif

using namespace std;


//char read[128];
//char ref[128];

char init_all_NULL[128] = "";
extern char *alignment_a, *alignment_b; 
//extern t_buf_pos alignment_max_length; 
//extern SCORING_SYSTEM* scoring; 

//char read_t[128] __aligned__;// = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
//char ref_t[128] __aligned__;// = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

int main(int argc, char* argv[]) {

    string *read_strs = new string [BATCH_RUN];
    string *ref_strs = new string [BATCH_RUN];
    bool *valid_buff = new bool [BATCH_RUN];

/*
    uint8_t **read0 = new uint8_t* [BATCH_RUN];
    uint8_t **read1 = new uint8_t* [BATCH_RUN];
    uint8_t **ref0 = new uint8_t* [BATCH_RUN];
    uint8_t **ref1 = new uint8_t* [BATCH_RUN];
    size_t *length = new size_t [BATCH_RUN];

    for (int i = 0; i < BATCH_RUN; i++) {
        read0[i] = new uint8_t[_MAX_LENGTH_ / 8];
        read1[i] = new uint8_t[_MAX_LENGTH_ / 8];
        ref0[i] = new uint8_t[_MAX_LENGTH_ / 8];
        ref1[i] = new uint8_t[_MAX_LENGTH_ / 8];
    }
*/
    char* seq1 = NULL, *seq2 = NULL;
    int algo_choose = 0; // default skipED
    if (argc < 2) {
        printf("Usage: $>bin error [NW(1) or skipED(0)]\n");
        exit(1);
    }

    int error = atoi(argv[1]);
    if (argc > 2) {
        algo_choose = atoi(argv[2]);
    }
    int use_SHD = -1; // use default as default. 
    if (argc > 3) {
        use_SHD = atoi(argv[3]); 
    }
    int use_levenshtein = 1; // use levenshtein as default. 
    if (argc > 4) { 
        use_levenshtein = atoi(argv[4]); 
    }
    if (algo_choose == 1) {
        set_default_scoring();
    }
    // ignore rest of arguments if existant

    size_t lineLength;
    char* tempstr = NULL;

    long long unsigned int passNum = 0;
    long long unsigned int totalNum = 0;

    long long read_size;
    long long read_idx;
    bool stop = false;

    tms start_time;
    tms end_time;
    tms elp_time;

    elp_time.tms_stime = 0;
    elp_time.tms_utime = 0;
    elp_time.tms_cstime = 0;
    elp_time.tms_cutime = 0;

    SIMD_ED ed_obj;
    if (use_levenshtein) { 
        bool tmp = (use_SHD == -1? true : use_SHD); 
        ed_obj.init_levenshtein(error, ED_GLOBAL, tmp);
    }
    else {
        bool tmp = (use_SHD == -1? false : use_SHD); 
        ed_obj.init_affine(error, error * 3, ED_GLOBAL, 2, 3, 1, tmp);
    }

    do {
        //clear past result
//      strncpy(read, init_all_NULL, 128);
//      strncpy(ref, init_all_NULL, 128);

        for (read_size = 0; read_size < BATCH_RUN; read_size++) {

            //get read
            getline(&tempstr, &lineLength, stdin);
            int length = strlen(tempstr) - 1;
            //length[read_size] = strlen(tempstr) - 1;
            //Get rid of the new line character
            tempstr[length] = '\0';

            if (strcmp(tempstr, "end_of_file\0") == 0) {
                stop = true;
                break;
            }
            read_strs[read_size].assign(tempstr);

            //get ref
            getline(&tempstr, &lineLength, stdin);
            length = strlen(tempstr) - 1;
            //length[read_size] = strlen(tempstr) - 1;
            //Get rid of the new line character
            tempstr[length] = '\0';
            ref_strs[read_size].assign(tempstr);
            valid_buff[read_size] = false;

            //if (length[read_size] > 128)
                //length[read_size] = 128;

            //cout << "length: " << length[read_size] << endl;

            //ed_obj.convert_reads((char*) read_strs[read_size].c_str(), (char*) ref_strs[read_size].c_str(),
                        //length[read_size], read0[read_size], read1[read_size], ref0[read_size], ref1[read_size]);
        }

        times(&start_time);

        for (read_idx = 0; read_idx < read_size; read_idx++) {

            // do the skipED affine.
            if (algo_choose == 0) {
                int length = read_strs[read_idx].length();
                ed_obj.load_reads((char*) read_strs[read_idx].c_str(), (char*) ref_strs[read_idx].c_str(), length);
                //ed_obj.load_reads(read0[read_idx], read1[read_idx], ref0[read_idx], ref1[read_idx], length[read_idx]);
                ed_obj.calculate_masks();
                ed_obj.reset();
                ed_obj.run();
                if (ed_obj.check_pass() ) {
                    //cout << "read_idx" << read_idx << endl;
                    //ed_obj.backtrack();
                    //fprintf(stderr, "%.*s\n", 128, ed_obj.get_CIGAR().c_str() );
                    valid_buff[read_idx] = true;
                }
                else {
                    //printf("fail read_num:%u\n", read_idx);
                }
            }
            // do the Needleman-Wunsch Affine ED
            //else if (algo_choose == 1) {
            //    seq1 = (char*)read_strs[read_idx].c_str();
            //    seq2 = (char*)ref_strs[read_idx].c_str();
            //    //printf("read_num:%u Before\n%s\n%s\n", read_idx, seq1, seq2);
            //    //printf("seq: %s seq2: %s\n", seq1, seq2);
            //    alignment_max_length = nw_alloc_mem(seq1, seq2, &alignment_a, &alignment_b);
            //    //printf("alignement_max_length: %llu\n", alignment_max_length);
            //    int tmp = align(seq1, seq2, NULL, NULL);
            //    //printf("align\n");
            //    if (tmp) {
            //        valid_buff[read_idx] = true;
            //        //printf("read_num:%u\n%s\n%s\n", read_idx, seq1, seq2);
            //    }
            //}

            // nw
            else if (algo_choose == 1) {
                seq1 = (const char*)read_strs[read_idx].c_str();
                seq2 = (const char*)ref_strs[read_idx].c_str();
                const parasail_matrix_t *user_matrix = parasail_matrix_create("ACGT", 0, -2);
                parasail_result_t* parasail_result;
                //parasail_result = parasail_nw_striped_sse41_128_16(seq1, (const int)read_strs[read_idx].length(), seq2, (const int)ref_strs[read_idx].length(), -3, -1, user_matrix);
                parasail_result = parasail_nw_banded(seq1, (const int)read_strs[read_idx].length(), seq2, (const int)ref_strs[read_idx].length(), -3, -1, 0, user_matrix);

                if (parasail_result->score >= (error * 2)) {
                    valid_buff[read_idx] = true;
                }
                //printf("%d ", parasail_result->score);
                parasail_result_free(parasail_result);
            }

            // nw  w/ SIMD
            else if (algo_choose == 2) {
                seq1 = (const char*)read_strs[read_idx].c_str();
                seq2 = (const char*)ref_strs[read_idx].c_str();
                const parasail_matrix_t *user_matrix = parasail_matrix_create("ACGT", 0, -2);
                parasail_result_t* parasail_result;
                parasail_result = parasail_nw_striped_sse41_128_16(seq1, (const int)read_strs[read_idx].length(), seq2, (const int)ref_strs[read_idx].length(), -3, -1, user_matrix);
                // parasail_result = parasail_nw_banded(seq1, (const int)read_strs[read_idx].length(), seq2, (const int)ref_strs[read_idx].length(), -3, -1, 0, user_matrix);

                if (parasail_result->score >= -(error * 2)) {
                    valid_buff[read_idx] = true;
                }
                //printf("%d ", parasail_result->score);
                parasail_result_free(parasail_result);
            }

/*
            else {
                fprintf(stdout, "error!\n");
                fprintf(stdout, "%.*s\n", 128, read_strs[read_idx].c_str() );
                fprintf(stdout, "%.*s\n", 128, ref_strs[read_idx].c_str() );
                exit(1);
            }
*/
            /*
            strncpy(read_t, init_all_NULL, 128);
            strncpy(ref_t, init_all_NULL, 128);

            length = read_strs[read_idx].length();

            if (length > 128)
                length = 128;
            strncpy(read_t, read_strs[read_idx].c_str(), length);

            length = ref_strs[read_idx].length();
            //Get rid of the new line character
            if (length > 128)
                length = 128;
            strncpy(ref_t, ref_strs[read_idx].c_str(), length);

            if (bit_vec_filter_sse1(read_t, ref_t, length, error))
                valid_buff[read_idx] = true;
            */
        }

        times(&end_time);

        for (read_idx = 0; read_idx < read_size; read_idx++) {

            if (valid_buff[read_idx]) {
                //fprintf(stderr, "%.*s\n", 128, read_strs[read_idx].c_str() );
                //fprintf(stderr, "%.*s\n", 128, ref_strs[read_idx].c_str() );
                passNum++;
            }
            totalNum++;
        }


        elp_time.tms_stime += end_time.tms_stime - start_time.tms_stime;
        elp_time.tms_utime += end_time.tms_utime - start_time.tms_utime;

        if (stop)
            break;

    } while (1);

    fprintf(stderr, "end_of_file\n");
    printf("passNum:\t%lld\n", passNum);
    printf("totalNum:\t%lld\n", totalNum);
    printf("total_time: %f\n", (double) elp_time.tms_utime / sysconf(_SC_CLK_TCK) );

    delete [] read_strs;
    delete [] ref_strs;
    delete [] valid_buff;

/*
    for (int i = 0 ; i < BATCH_RUN; i++) {
            delete [] read0[i];
            delete [] read1[i];
            delete [] ref0[i];
            delete [] ref1[i];
    }
    delete [] read0;
    delete [] read1;
    delete [] ref0;
    delete [] ref1;
    delete [] length;
*/

    return 0;

}

//#endif
