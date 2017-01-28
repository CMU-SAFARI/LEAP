/*
 nw_cmdline.c
 project: NeedlemanWunsch
 author: Isaac Turner <turner.isaac@gmail.com>
 url: http://sourceforge.net/projects/needlemanwunsch
 Copyright (C) 06-Dec-2011
 
 see: README

 == License
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <ctype.h> // tolower
//#include <stdarg.h> // required for va_list
//
//// my utility functions
//#include "string_buffer.h"
#include "bioinf.h"
//#include "utility_lib.h"
//
//// Alignment scoring and loading
//#include "alignment_scoring_load.h"
//
//#include "needleman_wunsch.h"
//
//// For this run
//char* cmd;
//char print_colour = 0, print_pretty = 0, print_scores = 0,
//     print_fasta = 0, print_zam = 0;
//
//SCORING_SYSTEM* scoring = NULL;
//
//// Alignment results stored here
//char *alignment_a = NULL, *alignment_b = NULL;
//t_buf_pos alignment_max_length;
//
//
void set_default_scoring(); 

void print_usage(char* err_fmt, ...); 

void align_zam(char *seq_a, char *seq_b); 

void align(char *seq_a, char *seq_b,
           char *seq_a_name, char *seq_b_name); 

// If seq2 is NULL, read pair of entries from first file
// Otherwise read an entry from each
void align_from_file(SEQ_FILE *seq1, SEQ_FILE *seq2); 

//int main(int argc, char* argv[])
//{
//  cmd = argv[0];
//
//  #ifdef DEBUG
//  printf("DEBUG: on\n");
//  #endif
//  
//  if(argc == 1)
//  {
//    print_usage(NULL);
//  }
//  
//  //
//  // Command line arguments handled here
//  //
//
//  char *seq1 = NULL, *seq2 = NULL;
//
//  char read_stdin = 0;
//
//  int file_list_length = 0;
//  int file_list_capacity = 10;
//  char** file_paths1 = (char**)malloc(sizeof(char*)*file_list_capacity);
//  char** file_paths2 = (char**)malloc(sizeof(char*)*file_list_capacity);
//
//  scoring = NULL;
//  char case_sensitive = 0;
//  
//  // First run through arguments to set up case_sensitive and scoring system
//
//  // case sensitive needs to be dealt with first
//  // (is is used to construct hash table for swap_table)
//  int argi;
//  for(argi = 1; argi < argc; argi++)
//  {
//    if(strcasecmp(argv[argi], "--help") == 0 ||
//       strcasecmp(argv[argi], "-help") == 0 ||
//       strcasecmp(argv[argi], "-h") == 0)
//    {
//      print_usage(NULL);
//    }
//    else if(strcasecmp(argv[argi], "--case_sensitive") == 0)
//    {
//      case_sensitive = 1;
//    }
//    else if(strcasecmp(argv[argi], "--scoring") == 0)
//    {
//      if(scoring != NULL)
//      {
//        print_usage("More than one scoring system specified - not permitted");
//      }
//    
//      if(strcasecmp(argv[argi+1], "PAM30") == 0)
//      {
//        scoring = scoring_system_PAM30();
//      }
//      else if(strcasecmp(argv[argi+1], "PAM70") == 0)
//      {
//        scoring = scoring_system_PAM70();
//      }
//      else if(strcasecmp(argv[argi+1], "BLOSUM80") == 0)
//      {
//        scoring = scoring_system_BLOSUM80();
//      }
//      else if(strcasecmp(argv[argi+1], "BLOSUM62") == 0)
//      {
//        scoring = scoring_system_BLOSUM62();
//      }
//      else if(strcasecmp(argv[argi+1], "DNA_HYBRIDIZATION") == 0)
//      {
//        scoring = scoring_system_DNA_hybridization();
//      }
//      else {
//        print_usage("Unknown --scoring choice, not one of "
//                    "PAM30|PAM70|BLOSUM80|BLOSUM62");
//      }
//
//      argi++; // took an argument
//    }
//  }
//
//  // Set up default scoring now
//  if(scoring == NULL)
//  {
//    set_default_scoring();
//  }
//
//  scoring->case_sensitive = case_sensitive;
//  // Scoring is now initiated - may tweak later
//
//  // Keep track of what is set
//  char substitutions_set = 0;
//  char match_set = 0;
//  char mismatch_set = 0;
//
//  for(argi = 1; argi < argc; argi++)
//  {
//    if(argv[argi][0] == '-')
//    {
//      // strcasecmp does case insensitive comparison
//      if(strcasecmp(argv[argi], "--freestartgap") == 0)
//      {
//        scoring->no_start_gap_penalty = 1;
//      }
//      else if(strcasecmp(argv[argi], "--freeendgap") == 0)
//      {
//        scoring->no_end_gap_penalty = 1;
//      }
//      else if(strcasecmp(argv[argi], "--case_sensitive") == 0)
//      {
//        // Already dealt with
//        //case_sensitive = 1;
//      }
//      else if(strcasecmp(argv[argi], "--printscores") == 0)
//      {
//        print_scores = 1;
//      }
//      else if(strcasecmp(argv[argi], "--printfasta") == 0)
//      {
//        print_fasta = 1;
//      }
//      else if(strcasecmp(argv[argi], "--pretty") == 0)
//      {
//        print_pretty = 1;
//      }
//      else if(strcasecmp(argv[argi], "--colour") == 0)
//      {
//        print_colour = 1;
//      }
//      else if(strcasecmp(argv[argi], "--zam") == 0)
//      {
//        print_zam = 1;
//      }
//      else if(strcasecmp(argv[argi], "--stdin") == 0)
//      {
//        read_stdin = 1;
//      }
//      else if(argi == argc-1)
//      {
//        // All the remaining options take an extra argument
//        print_usage("Unknown argument without parameter: %s",argv[argi]);
//      }
//      else if(strcasecmp(argv[argi], "--scoring") == 0)
//      {
//        // This handled above
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--substitution_matrix") == 0)
//      {
//        gzFile sub_matrix_file = gzopen(argv[argi+1], "r");
//        // gzbuffer(sub_matrix_file, 16384); // doesn't seem to work
//
//        align_scoring_load_matrix(sub_matrix_file, argv[argi+1],
//                                  scoring, case_sensitive);
//
//        //gzclose_r(sub_matrix_file); // doesn't seem to work
//        gzclose(sub_matrix_file);
//        substitutions_set = 1;
//
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--substitution_pairs") == 0)
//      {
//        gzFile sub_pairs_file = gzopen(argv[argi+1], "r");
//        //gzbuffer(sub_pairs_file, 16384); // doesn't seem to work
//        
//        align_scoring_load_pairwise(sub_pairs_file, argv[argi+1],
//                                    scoring, case_sensitive);
//        
//        //gzclose_r(sub_pairs_file); // doesn't seem to work
//        gzclose(sub_pairs_file);
//        substitutions_set = 1;
//
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--match") == 0)
//      {
//        if(!parse_entire_int(argv[argi+1], &scoring->match))
//        {
//          print_usage("Invalid --match argument ('%s') must be an int",
//                      argv[argi+1]);
//        }
//
//        match_set = 1;
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--mismatch") == 0)
//      {
//        if(!parse_entire_int(argv[argi+1], &scoring->mismatch))
//        {
//          print_usage("Invalid --mismatch argument ('%s') must be an int",
//                      argv[argi+1]);
//        }
//
//        mismatch_set = 1;
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--gapopen") == 0)
//      {
//        if(!parse_entire_int(argv[argi+1], &scoring->gap_open))
//        {
//          print_usage("Invalid --gapopen argument ('%s') must be an int",
//                      argv[argi+1]);
//        }
//
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--gapextend") == 0)
//      {
//        if(!parse_entire_int(argv[argi+1], &scoring->gap_extend))
//        {
//          print_usage("Invalid --gapextend argument ('%s') must be an int",
//                      argv[argi+1]);
//        }
//
//        argi++; // took an argument
//      }
//      else if(strcasecmp(argv[argi], "--file") == 0)
//      {
//        if(file_list_length == file_list_capacity)
//        {
//          // Expand arrays used for holding file paths
//          file_list_capacity *= 2;
//          file_paths1 = realloc(file_paths1, sizeof(char*)*file_list_capacity);
//          file_paths2 = realloc(file_paths2, sizeof(char*)*file_list_capacity);
//
//          if(file_paths1 == NULL || file_paths2 == NULL)
//          {
//            print_usage("Ran out of memory taking file arguments!\n");
//          }
//        }
//
//        file_paths1[file_list_length] = argv[argi+1];
//        file_paths2[file_list_length++] = NULL;
//        argi++; // took an argument
//      }
//      //else if(argi == argc-2)
//      //{
//        // All the remaining options take an 2 extra arguments
//        //unknown_option(argv[argi]);
//      //}
//      else if(strcasecmp(argv[argi], "--files") == 0)
//      {
//        if(file_list_length == file_list_capacity)
//        {
//          // Expand arrays used for holding file paths
//          file_list_capacity *= 2;
//          file_paths1 = realloc(file_paths1, sizeof(char*)*file_list_capacity);
//          file_paths2 = realloc(file_paths2, sizeof(char*)*file_list_capacity);
//
//          if(file_paths1 == NULL || file_paths2 == NULL)
//          {
//            print_usage("Ran out of memory taking file arguments!\n");
//          }
//        }
//
//        if(argi == argc-2)
//        {
//          print_usage("--files option takes 2 arguments");
//        }
//        else if(strcmp(argv[argi+1], "-") == 0 && strcmp(argv[argi+2], "-") == 0)
//        {
//          // Read both from stdin
//          file_paths1[file_list_length] = argv[argi+1];
//          file_paths2[file_list_length++] = NULL;
//        }
//        else
//        {
//          file_paths1[file_list_length] = argv[argi+1];
//          file_paths2[file_list_length++] = argv[argi+2];
//        }
//
//        argi += 2; // took two arguments
//      }
//      else
//      {
//        // Error - unknown option
//        print_usage("Unknown argument '%s'", argv[argi]);
//      }
//    }
//    else
//    {
//      if(argc - argi != 2)
//      {
//        print_usage("Unknown options: '%s'", argv[argi]);
//      }
//      break;
//    }
//  }
//
//  if(match_set != mismatch_set)
//  {
//    print_usage("--match --mismatch must both be set or neither set");
//  }
//  else if(substitutions_set && !match_set)
//  {
//    // if substitution table set and not match/mismatch
//    scoring->use_match_mismatch = 0;
//  }
//
//  // Check for extra unused arguments
//  // and set seq1 and seq2 if they have been passed
//  if(argi < argc)
//  {
//    seq1 = argv[argi];
//    seq2 = argv[argi+1];
//  }
//
//  if(seq1 == NULL && file_list_length == 0 && !read_stdin)
//  {
//    print_usage("No input specified");
//  }
//
//  if(print_zam && (print_pretty || print_scores || print_colour || print_fasta))
//  {
//    print_usage("Cannot use --printscore, --printfasta, --pretty or --colour "
//                "with --zam");
//  }
//  // End of set up
//
//  // Align!
//  if(seq1 != NULL)
//  {
//    // Align seq1 and seq2
//    alignment_max_length = nw_alloc_mem(seq1, seq2, &alignment_a, &alignment_b);
//    align(seq1, seq2, NULL, NULL);
//  }
//  else
//  {
//    // Set up default memory for aligning from stdin / files
//    alignment_max_length = 1000; // equivalent to two strings of 500bp
//    alignment_a = (char*) malloc((alignment_max_length+1) * sizeof(char));
//    alignment_b = (char*) malloc((alignment_max_length+1) * sizeof(char));
//  }
//
//  if(read_stdin)
//  {
//    // Read from STDIN
//    // Cannot turn off buffering in zlib, so have to use FILE for stdin
//    //gzFile gz_file = gzdopen(fileno(stdin), "r");
//    //gzsetparams(gz_file, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);
//    //SEQ_FILE* seq = seq_file_gzopen(gz_file);
//
//    SEQ_FILE* seq = seq_file_init(stdin);
//    align_from_file(seq, NULL);
//    seq_file_free(seq);
//
//    //gzclose(gz_file);
//  }
//
//  int i;
//  for(i = 0; i < file_list_length; i++)
//  {
//    // Read file(s)
//    SEQ_FILE *seq1 = seq_open_cmd_arg(file_paths1[i]);
//    SEQ_FILE *seq2 = seq_open_cmd_arg(file_paths2[i]);
//
//    // Align from files
//    align_from_file(seq1, seq2);
//
//    // Close files
//    seq_close_cmd_arg(seq1, file_paths1[i]);
//    seq_close_cmd_arg(seq2, file_paths2[i]);
//  }
//
//  // Free arrays of file paths
//  free(file_paths1);
//  free(file_paths2);
//
//  // Free memory for storing alignment results
//  free(alignment_a);
//  free(alignment_b);
//
//  scoring_free(scoring);
//
//  return EXIT_SUCCESS;
//}
