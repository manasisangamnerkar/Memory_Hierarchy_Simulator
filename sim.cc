#include "sim.h"
/*  "argc" holds the number of command-line arguments.
    "argv[]" holds the arguments themselves.

    Example:
    ./sim 32 8192 4 262144 8 3 10 gcc_trace.txt
    argc = 9
    argv[0] = "./sim"            - Simulator Name 
    argv[1] = "32"               - Blocksize 
    argv[2] = "8192"             - L1_size = 8Kb
    argv[3] = "4"                - L1_Assoc = 4
    argv[4] = "262144"           - L2_size = 256 KB 
    argv[5] = "8"                - L2_Assoc = 8
    argv[6] = "3"                - Pref_N = Number of Stream Buffers
    argv[7] = "10"               - Pref_M = Number memory blocks in Stream Buffer
    argv[8] = "gcc_trace.txt"    - Trace_File
*/
#include <cassert>
uint32_t main_mem_access = 0;

int main (int argc, char *argv[]) {
   FILE *fp;			// File pointer.
   char *trace_file;		// This variable holds the trace file name.
   cache_params_t params;	// Look at the sim.h header file for the definition of struct cache_params_t.
   char rw;			// This variable holds the request's type (read or write) obtained from the trace.
   uint32_t addr;		// This variable holds the request's address obtained from the trace.
				// The header file <inttypes.h> above defines signed and unsigned integers of various sizes in a machine-agnostic way.  "uint32_t" is an unsigned integer of 32 bits.

   // Exit with an error if the number of command-line arguments is incorrect.
   if (argc != 9) {
      printf("Error: Expected 8 command-line arguments but was provided %d.\n", (argc - 1));
      exit(EXIT_FAILURE);
   }
    
   // "atoi()" (included by <stdlib.h>) converts a string (char *) to an integer (int).
   params.BLOCKSIZE = (uint32_t) atoi(argv[1]);
   params.L1_SIZE   = (uint32_t) atoi(argv[2]);
   params.L1_ASSOC  = (uint32_t) atoi(argv[3]);
   params.L2_SIZE   = (uint32_t) atoi(argv[4]);
   params.L2_ASSOC  = (uint32_t) atoi(argv[5]);
   params.PREF_N    = (uint32_t) atoi(argv[6]);
   params.PREF_M    = (uint32_t) atoi(argv[7]);
   trace_file       = argv[8];

   // Open the trace file for reading.
   fp = fopen(trace_file, "r");
   if (fp == (FILE *) NULL) {
      // Exit with an error if file open failed.
      printf("Error: Unable to open file %s\n", trace_file);
      exit(EXIT_FAILURE);
   }
    
   // Print simulator configuration.
   printf("===== Simulator configuration =====\n");
   printf("BLOCKSIZE:  %u\n", params.BLOCKSIZE);
   printf("L1_SIZE:    %u\n", params.L1_SIZE);
   printf("L1_ASSOC:   %u\n", params.L1_ASSOC);
   printf("L2_SIZE:    %u\n", params.L2_SIZE);
   printf("L2_ASSOC:   %u\n", params.L2_ASSOC);
   printf("PREF_N:     %u\n", params.PREF_N);
   printf("PREF_M:     %u\n", params.PREF_M);
   printf("trace_file: %s\n", trace_file);
   printf("\n");

  
   //l1 cache object 
   l1_cache_t l1_cache_obj(params.BLOCKSIZE , params.L1_SIZE , params.L1_ASSOC , params.BLOCKSIZE , params.L2_SIZE , params.L2_ASSOC , params.PREF_M , params.PREF_N);
   

   // Read requests from the trace file and echo them back.
   while (fscanf(fp, "%c %x\n", &rw, &addr) == 2) {	// Stay in the loop if fscanf() successfully parsed two tokens as specified.
      if(rw == 'r')
      {
         l1_cache_obj.l1_read(addr);
     
      }
      else if (rw == 'w')
      {
         l1_cache_obj.l1_write(addr);
        
      }
      else 
      {
         printf("Error: Unknown request type %c.\n", rw);
	      exit(EXIT_FAILURE);
      }

      ///////////////////////////////////////////////////////
      // Issue the request to the L1 cache instance here.
      ///////////////////////////////////////////////////////

   
    }

    float l1_miss_rate;
    float l2_miss_rate = 0;
   
    l1_miss_rate = ((float)(l1_cache_obj.m_l1_read_misses)+(l1_cache_obj.m_l1_write_misses)) / ((l1_cache_obj.m_l1_reads)+(l1_cache_obj.m_l1_writes));


    //L1 contents display
    l1_cache_obj.display();
    if (params.L2_SIZE != 0)
    {
      printf("\n");
      l1_cache_obj.m_l2_cache_obj.display();
     
      l2_miss_rate = (((float)l1_cache_obj.m_l2_cache_obj.m_l2_read_misses)) / ((l1_cache_obj.m_l2_cache_obj.m_l2_reads));
    

    }
    
    //Measurement Display
      printf("\n===== Measurements =====\n");
      printf("a. L1 reads:                   %d\n",l1_cache_obj.m_l1_reads );
      printf("b. L1 read misses:             %d\n", l1_cache_obj.m_l1_read_misses );
      printf("c. L1 writes:                  %d\n", l1_cache_obj.m_l1_writes );
      printf("d. L1 write misses:            %d\n", l1_cache_obj.m_l1_write_misses);
      printf("e. L1 miss rate:               %0.4f\n", l1_miss_rate);
      printf("f. L1 writebacks:              %d\n", l1_cache_obj.m_l1_writebacks );
      printf("g. L1 prefetches:              %d\n", l1_cache_obj.m_l1_prefetches);
      printf("h. L2 reads (demand):          %d\n", l1_cache_obj.m_l2_cache_obj.m_l2_reads);
      printf("i. L2 read misses (demand):    %d\n", l1_cache_obj.m_l2_cache_obj.m_l2_read_misses);
      printf("j. L2 reads (prefetch):        %d\n", 0);
      printf("k. L2 read misses (prefetch):  %d\n", 0);
      printf("l. L2 writes:                  %d\n", l1_cache_obj.m_l2_cache_obj.m_l2_writes);
      printf("m. L2 write misses:            %d\n", l1_cache_obj.m_l2_cache_obj.m_l2_write_misses);
      printf("n. L2 miss rate:               %0.4f\n", l2_miss_rate);
      printf("o. L2 writebacks:              %d\n", l1_cache_obj.m_l2_cache_obj.m_l2_writebacks);
      printf("p. L2 prefetches:              %d\n", l1_cache_obj.m_l2_cache_obj.m_l2_prefetches);
      printf("q. memory traffic:             %d\n", (main_mem_access + l1_cache_obj.m_l2_cache_obj.m_l2_prefetches + l1_cache_obj.m_l1_prefetches));

    


    return(0);
}

/* 

            ************** DEFINITIONS ********************
*/

/* ************************************************ */
//       L1 CACHE CONSTRUCTOR DEFINITION
/* ************************************************ */

l1_cache_t::l1_cache_t(uint32_t l1_blocksize , uint32_t l1_size , uint32_t l1_assoc , uint32_t l2_blocksize , uint32_t l2_size , uint32_t l2_assoc , uint32_t pref_m , uint32_t pref_n)
{
   m_l1_blocksize = l1_blocksize;
   m_l1_size = l1_size;
   m_l1_assoc = l1_assoc;
   m_l1_pref_n = pref_n; 
   m_l1_pref_m = pref_m;  

   m_l1_num_sets = (m_l1_size) /((m_l1_blocksize) * (m_l1_assoc));
   m_l1_num_index_bits = log2(m_l1_num_sets);
   m_l1_num_blk_offset_bits = log2(m_l1_blocksize);
   m_l1_num_tag_bits = 32 - ((m_l1_num_index_bits) + (m_l1_num_blk_offset_bits));

   //memory initialization
   m_l1_memory =  new l1_memory_block_t*[m_l1_num_sets];
   for (uint32_t i = 0 ; i < m_l1_num_sets ; i++)
   {
      m_l1_memory[i] = new l1_memory_block_t[m_l1_assoc];
   }

   //lru value allocation
   for (uint32_t i = 0 ; i < m_l1_num_sets ; i++)
   {
      for (uint32_t j = 0 ; j < m_l1_assoc ; j++)
      {
         m_l1_memory[i][j].m_lru = j ;
      }
   }

   m_l2_cache_obj = l2_cache_t(l2_blocksize,l2_size,l2_assoc,pref_m ,pref_n);

   m_l1_stream_buffer = new uint32_t*[m_l1_pref_n];
   m_l1_sb_lru = new uint32_t[m_l1_pref_n];
   for(uint32_t i=0; i< m_l1_pref_n; i++){
      m_l1_sb_lru[i] = i;
      m_l1_stream_buffer[i] = new uint32_t[m_l1_pref_m];

      for(uint32_t j = 0 ; j < m_l1_pref_m ; j++)
      {
         m_l1_stream_buffer[i][j] = 0;
      }
   }  


   //reset l1 counters 
   m_l1_reads = 0;
   m_l1_read_misses = 0;
   m_l1_writes = 0;
   m_l1_write_misses = 0;
   m_l1_writebacks = 0;
   m_l1_prefetches = 0;
}


/* ************************************************ */
//     L1 MEMORY BLOCK CONSTRUCTOR DEFINITION
/* ************************************************ */

l1_memory_block_t::l1_memory_block_t()
{
   m_valid = false;
   m_dirty = false;
   m_tag_val = 0;
   m_lru = 0;
}

/* ************************************************ */
//            L1 CACHE READ DEFINITION
/* ************************************************ */

void l1_cache_t::l1_read(uint32_t addr)
{
   //step-1: read++

   m_l1_reads++;

   // step-2 : extract tag , index and block offset
   uint32_t addr_cp = addr;
   //uint32_t mask_block_offset;
   uint32_t mask_index;
   uint32_t mask_tag;
   uint32_t tag_val;
   uint32_t index_val;
   //uint32_t block_offset_val;

   //mask_block_offset = ((1<<m_l1_num_blk_offset_bits)-1);
   //block_offset_val = (addr_cp & mask_block_offset);

   addr_cp = addr_cp >> m_l1_num_blk_offset_bits;
   mask_index = ((1 << m_l1_num_index_bits)-1);
   index_val = (addr_cp & mask_index);

   addr_cp = addr_cp >> m_l1_num_index_bits;
   mask_tag = ((1 << m_l1_num_tag_bits )-1);
   tag_val = (addr_cp & mask_tag);

   //step : sb check (only when L1 cache)
   bool sb_hit=false;
   int sb_index = -1;
   uint32_t addr_block_addr  = addr >> m_l1_num_blk_offset_bits;

   if(m_l2_cache_obj.m_l2_size == 0){
      if ( (m_l1_pref_m > 0) && (m_l1_pref_n > 0) ) {

         // search in lru order 
         for(uint32_t i = 0; i < m_l1_pref_n; i++) {
            
         uint32_t curr_sb_index = 0; 

         // find index of lru sb    
         for(uint32_t j = 0; j < m_l1_pref_n; j++) {
            if ( m_l1_sb_lru[j] == i) {
               curr_sb_index = j; 
               break;
            }
         }
         // check current lru sb 
         for(uint32_t k = 0; k < m_l1_pref_m; k++) {
            if (m_l1_stream_buffer[curr_sb_index][k] == addr_block_addr){
               sb_hit = true;
               sb_index = curr_sb_index;
               break; 
               }
            }
            if ( sb_hit ){
               break; 
            }
         }
      }
   }   


   //step-3 : check if hit or miss
   bool f_hit;
   f_hit = false;
   for (uint32_t i=0 ; i < m_l1_assoc ; i++)
   {  

       /* ************************************************ */
       //                  READ HIT 
       /* ************************************************ */


      if ((m_l1_memory[index_val][i].m_tag_val == tag_val) && (m_l1_memory[index_val][i].m_valid == true))
      {  
         f_hit=true;

         uint32_t hit_arr_set_index;
         uint32_t hit_arr_assoc_index;

         hit_arr_set_index = index_val; // to store the index where hit took place 
         hit_arr_assoc_index = i ; // temporary variable to store the index 

         //lru_update 
                  for(uint32_t j=0 ; j < m_l1_assoc ; j++)
                  {
                     if((m_l1_memory[hit_arr_set_index][j].m_lru) < (m_l1_memory[hit_arr_set_index][hit_arr_assoc_index].m_lru))
                     {
                        m_l1_memory[hit_arr_set_index][j].m_lru ++;
                     }
                  }
               
         
         m_l1_memory[hit_arr_set_index][hit_arr_assoc_index].m_lru = 0;
         
         if (m_l2_cache_obj.m_l2_size == 0) {
            // cache hit sb hit 
            //stream buffer update 
            if((m_l1_pref_m > 0) && (m_l1_pref_n > 0))
            {
               if(sb_hit)
               {
                  uint32_t last_index_value;
                  last_index_value = m_l1_stream_buffer[sb_index][m_l1_pref_m-1];

                  for(uint32_t j = 0 ; j < m_l1_pref_m ; j++)
                  {
                     m_l1_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                     if (m_l1_stream_buffer[sb_index][j] > last_index_value)
                     {
                        m_l1_prefetches++ ;
                     }
                  }

                  // update lru. 
                  for(uint32_t i=0; i<m_l1_pref_n; i++) {
                     if ( m_l1_sb_lru[i] < m_l1_sb_lru[sb_index]) {
                        m_l1_sb_lru[i]++;
                     }
                  }
                  m_l1_sb_lru[sb_index] = 0;
               }
               else 
               {
                  //do nothing
               }
            }
         }

         break;         
      }
   
   }
   
   /* ************************************************ */
   //                  READ MISS 
   /* ************************************************ */
   
   if(f_hit == false)
   {  

      //step-2 : find maximum lru value 
      uint32_t max_lru_index = 0;

      for (uint32_t i = 0; i < m_l1_assoc ; i++)
      {
         if((m_l1_memory[index_val][i].m_lru) == (m_l1_assoc -1))
         {
            max_lru_index = i;
            break;
         }
      }

      //step-3 : handle victim block 
      //D = true 
      if((m_l1_memory[index_val][max_lru_index].m_dirty == true) && (m_l1_memory[index_val][max_lru_index].m_valid == true))
      {  
         //L2 is present 
         if(m_l2_cache_obj.m_l2_size != 0)
         {  
            uint32_t victim_addr ;
            uint32_t victim_tag = m_l1_memory[index_val][max_lru_index].m_tag_val;
            uint32_t victim_index = index_val;

            victim_addr = (victim_tag << ((m_l1_num_index_bits)+(m_l1_num_blk_offset_bits)));

            victim_addr = (victim_addr | (victim_index << m_l1_num_blk_offset_bits));

            m_l2_cache_obj.l2_write(victim_addr);
            m_l1_writebacks++ ;
         }

         //main memory only 
         else
         {
            //writeback to main memory
            main_mem_access++ ;
            m_l1_writebacks++ ;
         }
      }

      //step-4 : handle the actual block
      if(m_l2_cache_obj.m_l2_size != 0) //l1 + l2 (l2 may or maynot conatin pre fetcher)
      {
         m_l2_cache_obj.l2_read(addr);
         m_l1_read_misses++;
      }
      else // only l1
      {
         // step - sb hit / miss | cache miss 
         //stream buffer 
         if(m_l1_pref_m > 0 && m_l1_pref_n>0) // l1 + stream buffer
         {
            if(!sb_hit)
            {
               // increment read misses
               m_l1_read_misses++ ;
               main_mem_access++ ;

               //find maximum lru sb 
               uint32_t sb_max_lru_index = 0;

               for (uint32_t i = 0 ; i < m_l1_pref_n ; i++)
               {
                  if (m_l1_sb_lru[i] == ((m_l1_pref_n)-1) )
                  {
                     sb_max_lru_index = i ;
                     break;
                  }
               }

               
               for (uint32_t j = 0 ; j < m_l1_pref_m ; j++)
               {
                  m_l1_stream_buffer[sb_max_lru_index][j] = addr_block_addr + j + 1;
                  m_l1_prefetches++ ;
               }

               // update sb lru 
               for (uint32_t i=0; i<m_l1_pref_n; i++) {
                  if ( i != sb_max_lru_index ) {
                     m_l1_sb_lru[i]++;
                  }
               }
               m_l1_sb_lru[sb_max_lru_index] = 0; 

            }
            else 
            {
               uint32_t last_index_value;
               last_index_value = m_l1_stream_buffer[sb_index][m_l1_pref_m-1];

                     for(uint32_t j = 0 ; j < m_l1_pref_m ; j++)
                     {
                        m_l1_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                        if (m_l1_stream_buffer[sb_index][j] > last_index_value)
                        {
                           m_l1_prefetches++ ;
                        }

                     }

               for(uint32_t i=0; i<m_l1_pref_n; i++) {
               if ( m_l1_sb_lru[i] < m_l1_sb_lru[sb_index]) {
                     m_l1_sb_lru[i]++;
                  }
                  }
               m_l1_sb_lru[sb_index] = 0;

            }
         }
         else // only l1 
         {
            m_l1_read_misses++; 
            main_mem_access++;
         }
      }


      //step-5 : update the tag , lru

      m_l1_memory[index_val][max_lru_index].m_tag_val = tag_val ;
      m_l1_memory[index_val][max_lru_index].m_valid = true ;
      m_l1_memory[index_val][max_lru_index].m_dirty = false;

      //lru update 

      for(uint32_t i=0 ; i< m_l1_assoc ; i++)
      {
         if((m_l1_memory[index_val][i].m_lru) < (m_l1_memory[index_val][max_lru_index].m_lru))
         {
            m_l1_memory[index_val][i].m_lru++ ;
         }
      }

      m_l1_memory[index_val][max_lru_index].m_lru = 0;
   }
}

/* ************************************************ */
//            L1 CACHE WRITE DEFINITION
/* ************************************************ */

void l1_cache_t::l1_write(uint32_t addr)
{
   //step-1 : write++

   m_l1_writes++ ;

   //step-2 : extract tag, index and block offset 
   uint32_t addr_cp = addr;
   //uint32_t mask_block_offset;
   uint32_t mask_index;
   uint32_t mask_tag;
   uint32_t tag_val;
   uint32_t index_val;
   //uint32_t block_offset_val;

   //mask_block_offset = ((1<<m_l1_num_blk_offset_bits)-1);
   //block_offset_val = (mask_block_offset & addr_cp) ;

   addr_cp = (addr_cp >> m_l1_num_blk_offset_bits);
   mask_index = ((1<<m_l1_num_index_bits)-1);
   index_val = (addr_cp & mask_index);

   addr_cp = ( addr_cp >> m_l1_num_index_bits);
   mask_tag = ((1<<m_l1_num_tag_bits)-1);
   tag_val = (addr_cp & mask_tag);

   //step : sb check (only when L1 cache)
   bool sb_hit=false;
   int sb_index = -1;
   uint32_t addr_block_addr  = addr >> m_l1_num_blk_offset_bits;

   if(m_l2_cache_obj.m_l2_size == 0){
      if ( (m_l1_pref_m > 0) && (m_l1_pref_n > 0) ) {

         // search in lru order 
         for(uint32_t i = 0; i < m_l1_pref_n; i++) {
            
         uint32_t curr_sb_index = 0; 

         // find index of lru sb    
         for(uint32_t j = 0; j < m_l1_pref_n; j++) {
            if ( m_l1_sb_lru[j] == i) {
               curr_sb_index = j; 
               break;
            }
         }
         // check current lru sb 
         for(uint32_t k = 0; k < m_l1_pref_m; k++) {
            if (m_l1_stream_buffer[curr_sb_index][k] == addr_block_addr){
               sb_hit = true;
               sb_index = curr_sb_index;
               break; 
               }
            }
            if ( sb_hit ){
               break; 
            }
         }
      }
   }   

   //step-3 : check if hit or miss
   bool f_hit;
   f_hit = false;

   for (uint32_t i=0 ; i<m_l1_assoc ; i++)
   {
      /* ************************************************ */
      //                  WRITE HIT 
      /* ************************************************ */

      if((m_l1_memory[index_val][i].m_tag_val == tag_val) && (m_l1_memory[index_val][i].m_valid == true))
      {
         f_hit = true ;

         //update dirty bit as it can be first write to clean block
         m_l1_memory[index_val][i].m_dirty = true ;
         
         //lru update
         for(uint32_t j = 0 ; j < m_l1_assoc ; j++)
         {
            if((m_l1_memory[index_val][j].m_lru)<(m_l1_memory[index_val][i].m_lru))
            {
               m_l1_memory[index_val][j].m_lru++ ;
            }
         }
         
         m_l1_memory[index_val][i].m_lru = 0;

         if (m_l2_cache_obj.m_l2_size == 0) {
            // cache hit sb hit 
            //stream buffer update 
            if((m_l1_pref_m > 0) && (m_l1_pref_n > 0))
            {
               if(sb_hit)
               {
                  uint32_t last_index_value;
                  last_index_value = m_l1_stream_buffer[sb_index][m_l1_pref_m-1];

                  for(uint32_t j = 0 ; j < m_l1_pref_m ; j++)
                  {
                     m_l1_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                     if (m_l1_stream_buffer[sb_index][j] > last_index_value)
                     {
                        m_l1_prefetches++ ;
                     }
                  }

                  // update lru. 
                  for(uint32_t i=0; i<m_l1_pref_n; i++) {
                     if ( m_l1_sb_lru[i] < m_l1_sb_lru[sb_index]) {
                        m_l1_sb_lru[i]++;
                     }
                  }
                  m_l1_sb_lru[sb_index] = 0;
               }
               else 
               {
                  //do nothing
               }
            }
         }


         break;
      }

   }

   /* ************************************************ */
   //                  WRITE MISS 
   /* ************************************************ */

   if(f_hit == false)
   {


      //step-2 : index of maximum lru to be identified 
      uint32_t index_max_lru = 0 ;

      for(uint32_t i=0 ; i<m_l1_assoc ; i++)
      {
         if((m_l1_memory[index_val][i].m_lru) == (m_l1_assoc - 1))
         {
            index_max_lru = i;
            break;
         }
      }

      //step-3 : handle the victim block 
      if((m_l1_memory[index_val][index_max_lru].m_dirty == true) && (m_l1_memory[index_val][index_max_lru].m_valid==true))
      {
         m_l1_writebacks++ ;

         if(m_l2_cache_obj.m_l2_size != 0)
         {
            uint32_t victim_addr ;
            uint32_t victim_tag = m_l1_memory[index_val][index_max_lru].m_tag_val;
            uint32_t victim_index = index_val;

            victim_addr = (victim_tag << ((m_l1_num_index_bits)+(m_l1_num_blk_offset_bits)));

            victim_addr = (victim_addr | (victim_index << m_l1_num_blk_offset_bits));

            m_l2_cache_obj.l2_write(victim_addr);

         }
         else
         {
            main_mem_access++ ;
         }
      }

      //step-4 : handle the actual block 
      if(m_l2_cache_obj.m_l2_size != 0)
      {
         m_l2_cache_obj.l2_read(addr);
         m_l1_write_misses++;
      }
      else
      {
        // step - sb hit / miss | cache miss 
         //stream buffer 
         if(m_l1_pref_m > 0 && m_l1_pref_n>0)
         {
            if(!sb_hit)
            {
               // increment write misses
               m_l1_write_misses++ ;
               main_mem_access++ ;

               //find maximum lru sb 
               uint32_t sb_max_lru_index = 0;

               for (uint32_t i = 0 ; i < m_l1_pref_n ; i++)
               {
                  if (m_l1_sb_lru[i] == ((m_l1_pref_n)-1) )
                  {
                     sb_max_lru_index = i ;
                     break;
                  }
               }

               
               for (uint32_t j = 0 ; j < m_l1_pref_m ; j++)
               {
                  m_l1_stream_buffer[sb_max_lru_index][j] = addr_block_addr + j + 1;
                  m_l1_prefetches++ ;
               }

               // update sb lru 
               for (uint32_t i=0; i<m_l1_pref_n; i++) {
                  if ( i != sb_max_lru_index ) {
                     m_l1_sb_lru[i]++;
                  }
               }
               m_l1_sb_lru[sb_max_lru_index] = 0; 

            }
            else 
            {
               uint32_t last_index_value;
               last_index_value = m_l1_stream_buffer[sb_index][m_l1_pref_m-1];

                     for(uint32_t j = 0 ; j < m_l1_pref_m ; j++)
                     {
                        m_l1_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                        if (m_l1_stream_buffer[sb_index][j] > last_index_value)
                        {
                           m_l1_prefetches++ ;
                        }

                     }

               for(uint32_t i=0; i<m_l1_pref_n; i++) {
               if ( m_l1_sb_lru[i] < m_l1_sb_lru[sb_index]) {
                     m_l1_sb_lru[i]++;
                  }
                  }
               m_l1_sb_lru[sb_index] = 0;

            }
         }
         else 
         {
            m_l1_write_misses++; 
            main_mem_access++;
         }
      }


      //step-5 : tag , lru , dirty and valid update 
      m_l1_memory[index_val][index_max_lru].m_tag_val = tag_val;
      m_l1_memory[index_val][index_max_lru].m_dirty = true ;
      m_l1_memory[index_val][index_max_lru].m_valid = true ;

      for(uint32_t i=0 ; i < m_l1_assoc ; i++)
      {
         if((m_l1_memory[index_val][i].m_lru) < (m_l1_memory[index_val][index_max_lru].m_lru))
         {
            m_l1_memory[index_val][i].m_lru++;
         }
      }

      m_l1_memory[index_val][index_max_lru].m_lru = 0 ;
   }
}


void l1_cache_t::display()
{
   printf("===== L1 contents =====\n");
   for(uint32_t set=0 ; set < m_l1_num_sets ; set++ )
   {  
      printf("set      %4d:   ",set);

      //bubble sort based on lru 
      for(uint32_t i = 0 ; i < (m_l1_assoc -1) ; i++)
      {
         for(uint32_t j = 0 ; j < (m_l1_assoc-i-1) ; j++)
         {
            if(m_l1_memory[set][j].m_lru > m_l1_memory[set][j+1].m_lru)
            {  
               l1_memory_block_t temp ;
               temp = m_l1_memory[set][j];
               m_l1_memory[set][j] = m_l1_memory[set][j+1];
               m_l1_memory[set][j+1] = temp;
            }
         }
      }

      for(uint32_t assoc=0 ; assoc < m_l1_assoc ; assoc++)
      {  
         if(m_l1_memory[set][assoc].m_dirty == true)
         {
            printf("%x D ",m_l1_memory[set][assoc].m_tag_val);
         }
         else
         {
            printf("%x  ",m_l1_memory[set][assoc].m_tag_val);
         }
      }

      printf("\n");
   }

   
   if (m_l2_cache_obj.m_l2_size == 0)
   {
    if (m_l1_pref_m > 0 && m_l1_pref_n > 0)
   {
      printf("\n");
      printf("===== Stream Buffer(s) contents =====\n");
      for(uint32_t i=0; i<m_l1_pref_n; i++) {

         uint32_t lru_sb_index = 0; 

         for(uint32_t k=0; k<m_l1_pref_n; k++) {
            if (m_l1_sb_lru[k] == i){
               lru_sb_index = k;
               break;
            }
         }
         for(uint32_t j=0; j<m_l1_pref_m; j++){
         printf(" %x  ", m_l1_stream_buffer[lru_sb_index][j]);
         }
      printf("\n");
      }
   }
   }
}

// ******************************************************************************************
/* 

            ************** DEFINITIONS ********************
*/

/* ************************************************ */
//       L2 CACHE CONSTRUCTOR DEFINITION
/* ************************************************ */

l2_cache_t::l2_cache_t (uint32_t l2_blocksize , uint32_t l2_size , uint32_t l2_assoc , uint32_t l2_pref_m , uint32_t l2_pref_n)
{
  if(l2_size != 0)
  {
   m_l2_blocksize = l2_blocksize;
   m_l2_size = l2_size;
   m_l2_assoc = l2_assoc;
   m_l2_pref_m = l2_pref_m;
   m_l2_pref_n = l2_pref_n;

   m_l2_num_sets = (m_l2_size) /((m_l2_blocksize) * (m_l2_assoc));
   m_l2_num_index_bits = log2(m_l2_num_sets);
   m_l2_num_blk_offset_bits = log2(m_l2_blocksize);
   m_l2_num_tag_bits = 32 - ((m_l2_num_index_bits) + (m_l2_num_blk_offset_bits));

   //memory initialization
   m_l2_memory =  new l2_memory_block_t*[m_l2_num_sets];
   for (uint32_t i = 0 ; i < m_l2_num_sets ; i++)
   {
      m_l2_memory[i] = new l2_memory_block_t[m_l2_assoc];
   }

   //lru value allocation
   for (uint32_t i = 0 ; i < m_l2_num_sets ; i++)
   {
      for (uint32_t j = 0 ; j < m_l2_assoc ; j++)
      {
         m_l2_memory[i][j].m_lru = j ;
      }
   }

   //stream buffer initialisation 
   m_l2_stream_buffer = new uint32_t*[m_l2_pref_n];
   m_l2_sb_lru = new uint32_t [m_l2_pref_n];
   for(uint32_t i=0 ; i < m_l2_pref_n ; i++)
   {
      m_l2_sb_lru[i] = i;
      m_l2_stream_buffer[i] = new uint32_t[m_l2_pref_m];

      for(uint32_t j = 0 ; j < m_l2_pref_m ; j++)
      {
         m_l2_stream_buffer[i][j] = 0;
      }
   }
 }

   //reset l2 counters 
   m_l2_reads = 0;
   m_l2_read_misses = 0;
   m_l2_writes = 0;
   m_l2_write_misses = 0;
   m_l2_writebacks = 0;
   m_l2_prefetches = 0;
}


//default constructor 

l2_cache_t::l2_cache_t()
{

}

/* ************************************************ */
//     L2 MEMORY BLOCK CONSTRUCTOR DEFINITION
/* ************************************************ */

l2_memory_block_t::l2_memory_block_t()
{
   m_valid = false;
   m_dirty = false;
   m_tag_val = 0;
   m_lru = 0;
}

/* ************************************************ */
//            L2 CACHE READ DEFINITION
/* ************************************************ */

void l2_cache_t::l2_read (uint32_t addr)
{
   //step-1: read++

   m_l2_reads++;

   // step-2 : extract tag , index and block offset
   uint32_t addr_cp = addr;
   //uint32_t mask_block_offset;
   uint32_t mask_index;
   uint32_t mask_tag;
   uint32_t tag_val;
   uint32_t index_val;
   //uint32_t block_offset_val;

   //mask_block_offset = ((1<<m_l2_num_blk_offset_bits)-1);
   //block_offset_val = (addr_cp & mask_block_offset);

   addr_cp = addr_cp >> m_l2_num_blk_offset_bits;
   mask_index = ((1 << m_l2_num_index_bits)-1);
   index_val = (addr_cp & mask_index);

   addr_cp = addr_cp >> m_l2_num_index_bits;
   mask_tag = ((1 << m_l2_num_tag_bits )-1);
   tag_val = (addr_cp & mask_tag);

   //step - sb check 
   bool sb_hit=false;
   int sb_index = -1;
   uint32_t addr_block_addr = addr >> m_l2_num_blk_offset_bits;

   if ( (m_l2_pref_m > 0) && (m_l2_pref_n > 0) ) {

      // search in lru order 
      for(uint32_t i = 0; i < m_l2_pref_n; i++) {
         
         uint32_t curr_sb_index = 0; 

         // find index of current lru sb    
         for(uint32_t j = 0; j < m_l2_pref_n; j++) {
            if ( m_l2_sb_lru[j] == i) {
               curr_sb_index = j; 
               break;
            }
         }

         // check current lru sb 
         for(uint32_t k = 0; k < m_l2_pref_m; k++) {
            if (m_l2_stream_buffer[curr_sb_index][k] == addr_block_addr){
               sb_hit = true;
               sb_index = curr_sb_index;
               break; 
               
            }
         }

         if ( sb_hit ){
            break; 
         }
      }

   }
   
   //step-3 : check if hit or miss
   bool f_hit;
   f_hit = false;
   for (uint32_t i=0 ; i < m_l2_assoc ; i++)
   {  

       /* ************************************************ */
       //                  READ HIT 
       /* ************************************************ */


      if ((m_l2_memory[index_val][i].m_tag_val == tag_val) && (m_l2_memory[index_val][i].m_valid == true))
      {  
         f_hit=true;

         uint32_t hit_arr_set_index;
         uint32_t hit_arr_assoc_index;

         hit_arr_set_index = index_val; // to store the index where hit took place 
         hit_arr_assoc_index = i ; // temporary variable to store the index 

         //lru_update 
                  for(uint32_t j=0 ; j < m_l2_assoc ; j++)
                  {
                     if((m_l2_memory[hit_arr_set_index][j].m_lru) < (m_l2_memory[hit_arr_set_index][hit_arr_assoc_index].m_lru))
                     {
                        m_l2_memory[hit_arr_set_index][j].m_lru ++;
                     }
                  }
               
         
         m_l2_memory[hit_arr_set_index][hit_arr_assoc_index].m_lru = 0;

         //stream buffer update 
         if((m_l2_pref_m > 0) && (m_l2_pref_n > 0))
         {
            if(sb_hit) // cache hit + sb hit 
            {
               uint32_t last_index_value;
               last_index_value = m_l2_stream_buffer[sb_index][m_l2_pref_m-1];


               for(uint32_t j = 0 ; j < m_l2_pref_m ; j++)
               {
                  m_l2_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                  if (m_l2_stream_buffer[sb_index][j] > last_index_value)
                  {  
                      m_l2_prefetches++ ;
                  }

               }

               // update lru. 
               for(uint32_t i=0; i<m_l2_pref_n; i++) {
                  if ( m_l2_sb_lru[i] < m_l2_sb_lru[sb_index]) {
                     m_l2_sb_lru[i]++;
                  }
               }
               m_l2_sb_lru[sb_index] = 0;

            }
            else // cache hit + sb miss 
            {
               //do nothing
            }
         }

         break;         
      }
   
   }
   
   /* ************************************************ */
   //                  READ MISS 
   /* ************************************************ */
   
   if(f_hit == false) // cache miss. 
   {  
      
      //step-2 : find maximum lru value 
      uint32_t max_lru_index = 0;

      for (uint32_t i = 0; i < m_l2_assoc ; i++)
      {
         if((m_l2_memory[index_val][i].m_lru) == (m_l2_assoc -1))
         {
            max_lru_index = i;
            break;
         }
      }

      //step-3 : dirty bit check  
      if((m_l2_memory[index_val][max_lru_index].m_dirty == true) && (m_l2_memory[index_val][max_lru_index].m_valid == true))
      {  
         //writeback to main memory
         main_mem_access++ ;
         m_l2_writebacks++ ;
      }

      //step-4 : update the tag , lru

      m_l2_memory[index_val][max_lru_index].m_tag_val = tag_val ;
      m_l2_memory[index_val][max_lru_index].m_valid = true ;
      m_l2_memory[index_val][max_lru_index].m_dirty = false;

      //lru update 
      for(uint32_t i=0 ; i< m_l2_assoc ; i++)
      {
         if((m_l2_memory[index_val][i].m_lru) < (m_l2_memory[index_val][max_lru_index].m_lru))
         {
            m_l2_memory[index_val][i].m_lru++ ;
         }
      }
      m_l2_memory[index_val][max_lru_index].m_lru = 0;
      
      if(m_l2_pref_m > 0 && m_l2_pref_n>0)
      {
         if(!sb_hit) // cache miss + sb miss
         {
            // L2 related increment read misses
            m_l2_read_misses++ ;
            main_mem_access++ ;

            //find maximum lru sb 
            uint32_t sb_max_lru_index = 0;

            for (uint32_t i = 0 ; i < m_l2_pref_n ; i++)
            {
               if (m_l2_sb_lru[i] == ((m_l2_pref_n)-1) )
               {
                  sb_max_lru_index = i ;
                  break;
               }
            }

      
            for (uint32_t j = 0 ; j < m_l2_pref_m ; j++)
            {
               m_l2_stream_buffer[sb_max_lru_index][j] = addr_block_addr + 1 + j;
               m_l2_prefetches++ ;
            }

            // update sb lru 
            for (uint32_t i=0; i<m_l2_pref_n; i++) {
               if ( i != sb_max_lru_index ) {
                  m_l2_sb_lru[i]++;
               }
            }
            m_l2_sb_lru[sb_max_lru_index] = 0; 

         }
         else // cache miss + sb hit
         {
            uint32_t last_index_value;
            last_index_value = m_l2_stream_buffer[sb_index][m_l2_pref_m-1];

            for(uint32_t j = 0 ; j < m_l2_pref_m ; j++)
            {  
               uint32_t addr_sb_add = addr_block_addr + j + 1; 
               m_l2_stream_buffer[sb_index][j] = addr_sb_add;

               if (addr_sb_add > last_index_value)
               {
                  m_l2_prefetches++ ;
               }

            }

            for(uint32_t i=0; i<m_l2_pref_n; i++) {
                if ( m_l2_sb_lru[i] < m_l2_sb_lru[sb_index]) {
                  m_l2_sb_lru[i]++;
                }
            }
            m_l2_sb_lru[sb_index] = 0;



         }
      } 
      else // no stream buffer.
      {
         m_l2_read_misses++; 
         main_mem_access++;
      }
      
   }
}

/* ************************************************ */
//            L2 CACHE WRITE DEFINITION
/* ************************************************ */

void l2_cache_t::l2_write(uint32_t addr)
{
   //step-1 : write++

   m_l2_writes++ ;

   //step-2 : extract tag, index and block offset 
   uint32_t addr_cp = addr;
   //uint32_t mask_block_offset;
   uint32_t mask_index;
   uint32_t mask_tag;
   uint32_t tag_val;
   uint32_t index_val;
   //uint32_t block_offset_val;

   //mask_block_offset = ((1<<m_l2_num_blk_offset_bits)-1);
   //block_offset_val = (mask_block_offset & addr_cp) ;

   addr_cp = (addr_cp >> m_l2_num_blk_offset_bits);
   mask_index = ((1<<m_l2_num_index_bits)-1);
   index_val = (addr_cp & mask_index);

   addr_cp = ( addr_cp >> m_l2_num_index_bits);
   mask_tag = ((1<<m_l2_num_tag_bits)-1);
   tag_val = (addr_cp & mask_tag);

   //step - sb check 
   bool sb_hit=false;
   int sb_index = -1;
   uint32_t addr_block_addr  = addr >> m_l2_num_blk_offset_bits;

   if ( (m_l2_pref_m > 0) && (m_l2_pref_n > 0) ) {

      // search in lru order 
      for(uint32_t i = 0; i < m_l2_pref_n; i++) {
         
         uint32_t curr_sb_index = 0; 

         // find index of lru sb    
         for(uint32_t j = 0; j < m_l2_pref_n; j++) {
            if ( m_l2_sb_lru[j] == i) {
               curr_sb_index = j; 
               break;
            }
         }

         // check current lru sb 
         for(uint32_t k = 0; k < m_l2_pref_m; k++) {
            if (m_l2_stream_buffer[curr_sb_index][k] == addr_block_addr){
               sb_hit = true;
               sb_index = curr_sb_index;
               break; 
               
            }
         }

         if ( sb_hit ){
            break; 
         }
      }


   }
   
   
   //step-3 : check if hit or miss
   bool f_hit;
   f_hit = false;

   for (uint32_t i=0 ; i<m_l2_assoc ; i++)
   {
      /* ************************************************ */
      //                  WRITE HIT 
      /* ************************************************ */

      if((m_l2_memory[index_val][i].m_tag_val == tag_val) && (m_l2_memory[index_val][i].m_valid == true))
      {
         f_hit = true ;
         m_l2_memory[index_val][i].m_dirty = true ;
         
         //lru update
         for(uint32_t j = 0 ; j < m_l2_assoc ; j++)
         {
            if((m_l2_memory[index_val][j].m_lru)<(m_l2_memory[index_val][i].m_lru))
            {
               m_l2_memory[index_val][j].m_lru++ ;
            }
         }
         
         m_l2_memory[index_val][i].m_lru = 0;

         //stream buffer update 
         if((m_l2_pref_m > 0) && (m_l2_pref_n > 0))
         {
            if(sb_hit)
            {
               uint32_t last_index_value;
               last_index_value = m_l2_stream_buffer[sb_index][m_l2_pref_m-1];

               for(uint32_t j = 0 ; j < m_l2_pref_m ; j++)
               {
                  m_l2_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                  if (m_l2_stream_buffer[sb_index][j] > last_index_value)
                  {
                      m_l2_prefetches++ ;
                  }

               }

               // update lru. 
               for(uint32_t i=0; i<m_l2_pref_n; i++) {
                  if ( m_l2_sb_lru[i] < m_l2_sb_lru[sb_index]) {
                     m_l2_sb_lru[i]++;
                  }
               }
               m_l2_sb_lru[sb_index] = 0;
            }
            else 
            {
               //do nothing
            }
         }

         break;
      }

   }

   /* ************************************************ */
   //                  WRITE MISS 
   /* ************************************************ */

   if(f_hit == false)
   {
      

      //step-2 : index of maximum lru to be identified 
      uint32_t index_max_lru = 0 ;

      for(uint32_t i=0 ; i<m_l2_assoc ; i++)
      {
         if((m_l2_memory[index_val][i].m_lru) == (m_l2_assoc - 1))
         {
            index_max_lru = i;
            break;
         }
      }

      //step-3 : for D=1
      if((m_l2_memory[index_val][index_max_lru].m_dirty == true) && (m_l2_memory[index_val][index_max_lru].m_valid==true))
      {
         m_l2_writebacks++ ;
         main_mem_access++;
      }

      //step-4 : tag , lru , dirty , main_mem_access and valid update 
      m_l2_memory[index_val][index_max_lru].m_tag_val = tag_val;
      m_l2_memory[index_val][index_max_lru].m_dirty = true ;
      m_l2_memory[index_val][index_max_lru].m_valid = true ;

      for(uint32_t i=0 ; i < m_l2_assoc ; i++)
      {
         if((m_l2_memory[index_val][i].m_lru) < (m_l2_memory[index_val][index_max_lru].m_lru))
         {
            m_l2_memory[index_val][i].m_lru++;
         }
      }

      m_l2_memory[index_val][index_max_lru].m_lru = 0 ;

      //stream buffer 
      if(m_l2_pref_m > 0 && m_l2_pref_n>0)
      {
         if(!sb_hit)
         {
            // increment read misses
            m_l2_write_misses++ ;
            main_mem_access++ ;

            //find maximum lru sb 
            uint32_t sb_max_lru_index = 0;

            for (uint32_t i = 0 ; i < m_l2_pref_n ; i++)
            {
               if (m_l2_sb_lru[i] == ((m_l2_pref_n)-1) )
               {
                  sb_max_lru_index = i ;
                  break;
               }
            }

            
            for (uint32_t j = 0 ; j < m_l2_pref_m ; j++)
            {
               m_l2_stream_buffer[sb_max_lru_index][j] = addr_block_addr + j + 1;
               m_l2_prefetches++ ;
            }

            // update sb lru 
            for (uint32_t i=0; i<m_l2_pref_n; i++) {
               if ( i != sb_max_lru_index ) {
                  m_l2_sb_lru[i]++;
               }
            }
            m_l2_sb_lru[sb_max_lru_index] = 0; 

         }
         else 
         {
            uint32_t last_index_value;
            last_index_value = m_l2_stream_buffer[sb_index][m_l2_pref_m-1];

                  for(uint32_t j = 0 ; j < m_l2_pref_m ; j++)
                  {
                     m_l2_stream_buffer[sb_index][j] = addr_block_addr + (j+1);

                     if (m_l2_stream_buffer[sb_index][j] > last_index_value)
                     {
                        m_l2_prefetches++ ;
                     }

                  }

            for(uint32_t i=0; i<m_l2_pref_n; i++) {
            if ( m_l2_sb_lru[i] < m_l2_sb_lru[sb_index]) {
                  m_l2_sb_lru[i]++;
                }
               }
            m_l2_sb_lru[sb_index] = 0;

         }
      }
      else 
      {
         m_l2_write_misses++; 
         main_mem_access++;
      }
   }

}


void l2_cache_t::display()
{
   printf("===== L2 contents =====\n");
   for(uint32_t set=0 ; set < m_l2_num_sets ; set++ )
   {  
      printf("set      %4d:   ",set);

      //bubble sort based on lru 
      for(uint32_t i = 0 ; i < (m_l2_assoc -1) ; i++)
      {
         for(uint32_t j = 0 ; j < (m_l2_assoc-i-1) ; j++)
         {
            if(m_l2_memory[set][j].m_lru > m_l2_memory[set][j+1].m_lru)
            {  
               l2_memory_block_t temp ;
               temp = m_l2_memory[set][j];
               m_l2_memory[set][j] = m_l2_memory[set][j+1];
               m_l2_memory[set][j+1] = temp;
            }
         }
      }

      for(uint32_t assoc=0 ; assoc < m_l2_assoc ; assoc++)
      {  
         if(m_l2_memory[set][assoc].m_dirty == true)
         {
            printf("%x D   ",m_l2_memory[set][assoc].m_tag_val);
         }
         else
         {
            printf("%x     ",m_l2_memory[set][assoc].m_tag_val);
         }
      }

      printf("\n");
   }


   if (m_l2_pref_m > 0 && m_l2_pref_n > 0)
   {
      printf("\n");
      printf("===== Stream Buffer(s) contents =====\n");
      for(uint32_t i=0; i<m_l2_pref_n; i++) {

         uint32_t lru_sb_index = 0; 

         for(uint32_t k=0; k<m_l2_pref_n; k++) {
            if (m_l2_sb_lru[k] == i){
               lru_sb_index = k;
               break;
            }
         }
         for(uint32_t j=0; j<m_l2_pref_m; j++){
         printf(" %x  ", m_l2_stream_buffer[lru_sb_index][j]);
         }
      printf("\n");
      }
   }
}