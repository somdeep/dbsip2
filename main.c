#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "p2random.h"
#include "tree.h"


#include <xmmintrin.h>
#include <emmintrin.h> 
#include <pmmintrin.h> 
#include <tmmintrin.h> 
#include <smmintrin.h> 
#include <nmmintrin.h> 
#include <ammintrin.h> 
#include <x86intrin.h>

void split32(__m128i x)
{
        int32_t *pt = (int32_t*) &x;
       // printf("\nPrinting values : \n");
        printf("%d %d %d %d\n", x[0],x[1],x[2],x[3]);

}

//GENERAL PROBE
int generalprobe(Tree* tree, size_t *fanout, int32_t probe) 
{
        // int32_t currentLevel, int32_t prev_result, int32_t probe, int32_t fanout) {
        
        int32_t currentLevel = 0, prev_result = 0,res;
        int32_t** treeLevels = tree->key_array;
        int32_t num_levels = tree->num_levels;
        __m128i key =  _mm_cvtsi32_si128(probe);
        split32(key);
        key = _mm_shuffle_epi32(key, 0);
        printf("\nNumber of levels : %d\n",num_levels);
        for(currentLevel=0;currentLevel < num_levels ; currentLevel++) {

                
                //-----probe level with fan out of 5---------
                if(fanout[currentLevel]==5) {         
                        __m128i lvl = _mm_load_si128(/**/ (__m128i*)&treeLevels[currentLevel][prev_result << 2]);
                        __m128i cmp_1 = _mm_cmpgt_epi32(key,lvl);
                        __m128 new_cmp = _mm_castsi128_ps(cmp_1);
                        int mask = _mm_movemask_ps(new_cmp);   // ps: epi32
                        printf("Mask : %d\n",mask);
                        res = _bit_scan_forward(mask ^ 0x1FF);
                        printf("res : %d\n",res);
                        res += (prev_result << 2) + prev_result;
                        printf("res : %d\n",res);
                        //return res;
                }
                //-----probe level with fan out of 9---------
                else if (fanout[currentLevel]==9) {
                        __m128i lvl_A = _mm_load_si128(( __m128i*)&treeLevels[currentLevel][ prev_result << 3]);
                        __m128i lvl_B = _mm_load_si128(( __m128i*)&treeLevels[currentLevel][(prev_result << 3) + 4]);
                        __m128i cmp_A = _mm_cmpgt_epi32(key,lvl_A);
                        __m128i cmp_B = _mm_cmpgt_epi32(key,lvl_B);
                        __m128i cmp = _mm_packs_epi32(cmp_A, cmp_B);
                        cmp = _mm_packs_epi16(cmp, _mm_setzero_si128());
                        int mask = _mm_movemask_epi8(cmp);
                        res = _bit_scan_forward(mask ^ 0x1FFFF);
                        res += (prev_result << 3) + prev_result;
                        //return res;
                }
                //-----probe level with fan out of 17---------
                else if(fanout[currentLevel]==17) {
                        
                }
                prev_result = res;
                printf("Yo mama so fat : \n %d \n", res);
                //break;

        }
        return res;
}


//HARD-CODED PROBE IMPLEMENTATION


int main(int argc, char* argv[]) {
        // parsing arguments
        assert(argc > 3);
        size_t num_keys = strtoull(argv[1], NULL, 0);
        size_t num_probes = strtoull(argv[2], NULL, 0);
        size_t num_levels = (size_t) argc - 3;
        size_t* fanout = malloc(sizeof(size_t) * num_levels);
        assert(fanout != NULL);
        for (size_t i = 0; i < num_levels; ++i) {
                fanout[i] = strtoull(argv[i + 3], NULL, 0);
                assert(fanout[i] >= 2 && fanout[i] <= 17);
        }

        // building the tree index
        rand32_t* gen = rand32_init((uint32_t) time(NULL));
        assert(gen != NULL);
        int32_t* delimiter = generate_sorted_unique(num_keys, gen);
        assert(delimiter != NULL);
        Tree* tree = build_index(num_levels, fanout, num_keys, delimiter);
        free(delimiter);
        //free(fanout);
        if (tree == NULL) {
                free(gen);
                exit(EXIT_FAILURE);
        }

        // generate probes
        int32_t* probe = generate(num_probes, gen);
        assert(probe != NULL);
        free(gen);
        uint32_t* result = malloc(sizeof(uint32_t) * num_probes);
        assert(result != NULL);

        // perform index probing (Phase 2)
        for (size_t i = 0; i < num_probes; ++i) {
                result[i] = generalprobe(tree, fanout, probe[i]);
                break;
                
        }

        // output results
        for (size_t i = 0; i < num_probes; ++i) {
                fprintf(stdout, "%d %u\n", probe[i], result[i]);
                
        }

        // cleanup and exit
        free(result);
        free(probe);
        cleanup_index(tree);
        return EXIT_SUCCESS;
}