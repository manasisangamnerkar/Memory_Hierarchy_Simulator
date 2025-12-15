#define ADDRESS_SIZE 32
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include<cmath>
#include<vector>
using namespace std;

typedef 
struct {
   uint32_t BLOCKSIZE;
   uint32_t L1_SIZE;
   uint32_t L1_ASSOC;
   uint32_t L2_SIZE;
   uint32_t L2_ASSOC;
   uint32_t PREF_N;
   uint32_t PREF_M;
} cache_params_t;

struct l2_memory_block_t{

      //parameters 
      bool m_valid;
      bool m_dirty;
      uint32_t m_tag_val;
      uint32_t m_lru;

      //constructor declaration 
      l2_memory_block_t();
};

struct l2_cache_t{

   //input by simulator
   uint32_t m_l2_blocksize;
   uint32_t m_l2_size;
   uint32_t m_l2_assoc;
   uint32_t m_l2_pref_m;
   uint32_t m_l2_pref_n;

   //parameters 

   uint32_t m_l2_num_sets;
   uint32_t m_l2_num_index_bits;
   uint32_t m_l2_num_blk_offset_bits;
   uint32_t m_l2_num_tag_bits;

   //l2 counters 

   uint32_t m_l2_reads;
   uint32_t m_l2_read_misses;
   uint32_t m_l2_writes;
   uint32_t m_l2_write_misses;
   uint32_t m_l2_writebacks;
   uint32_t m_l2_prefetches;

   //memory parameters
   l2_memory_block_t **m_l2_memory;   
   //l2_memory_block_t m_l2_memory [sets][assoc]

   uint32_t **m_l2_stream_buffer;
   uint32_t *m_l2_sb_lru;


   //constructor declaration 
   l2_cache_t(uint32_t l2_blocksize , uint32_t l2_size , uint32_t l2_assoc , uint32_t l2_pref_m , uint32_t l2_pref_n);
   l2_cache_t();


   //functions of l2 cache
   void l2_read(uint32_t addr);
   void l2_write(uint32_t addr);
   void display();
};

struct l1_memory_block_t{

      //parameters 
      bool m_valid;
      bool m_dirty;
      uint32_t m_tag_val;
      uint32_t m_lru;

      //constructor declaration 
      l1_memory_block_t();
};

struct l1_cache_t{

   //input by simulator
   uint32_t m_l1_blocksize;
   uint32_t m_l1_size;
   uint32_t m_l1_assoc;
   uint32_t m_l1_pref_n; 
   uint32_t m_l1_pref_m; 

   //parameters 

   uint32_t m_l1_num_sets;
   uint32_t m_l1_num_index_bits;
   uint32_t m_l1_num_blk_offset_bits;
   uint32_t m_l1_num_tag_bits;

   //l1 counters 

   uint32_t m_l1_reads;
   uint32_t m_l1_read_misses;
   uint32_t m_l1_writes;
   uint32_t m_l1_write_misses;
   uint32_t m_l1_writebacks;
   uint32_t m_l1_prefetches;

   //object of L2
   l2_cache_t m_l2_cache_obj;

   //memory parameters
   l1_memory_block_t **m_l1_memory;   
   //l1_memory_block_t m_l1_memory [sets][assoc]


   //stream buffer 
   uint32_t **m_l1_stream_buffer; // m_l1_stream_buffer[N]][M]
   uint32_t *m_l1_sb_lru; // m_l1_sb_lru[N]


   //constructor declaration 
   l1_cache_t(uint32_t l1_blocksize , uint32_t l1_size , uint32_t l1_assoc , uint32_t l2_blocksize , uint32_t l2_size , uint32_t l2_assoc , uint32_t pref_m , uint32_t pref_n);

   //functions of l1 cache
   void l1_read(uint32_t addr);
   void l1_write(uint32_t addr);
   void display();
};