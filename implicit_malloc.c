// /*
//  * mm-naive.c - The fastest, least memory-efficient malloc package.
//  * 
//  * In this naive approach, a block is allocated by simply incrementing
//  * the brk pointer.  A block is pure payload. There are no headers or
//  * footers.  Blocks are never coalesced or reused. Realloc is
//  * implemented directly using mm_malloc and mm_free.
//  *
//  * NOTE TO STUDENTS: Replace this header comment with your own header
//  * comment that gives a high level description of your solution.
//  */
// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <unistd.h>
// #include <string.h>
// #include "mm.h"
// #include "memlib.h"

// /*********************************************************
//  * NOTE TO STUDENTS: Before you do anything else, please
//  * provide your team information in the following struct.
//  ********************************************************/
// team_t team = {
//     /* Team name */
//     "team_5",
//     /* First member's full name */
//     "Gyeongwon_Seo",
//     /* First member's email address */
//     "tjsxor50@gmail.com",
//     /* Second member's full name (leave blank if none) */
//     "",
//     /* Second member's email address (leave blank if none) */
//     ""
// };

// /* single word (4) or double word (8) alignment */
// #define ALIGNMENT 8

// /* rounds up to the nearest multiple of ALIGNMENT */
// #define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

//  /* Basic constants and macros */
// #define WSIZE 8 /* Word and header/footer size (bytes) */
// #define DSIZE 16 /* Double word size (bytes) */
// #define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */
// #define MAX(x, y) ((x) > (y)? (x) : (y))
// /* Pack a size and allocated bit into a word */
// #define PACK(size, alloc) ((size) | (alloc))    

//  /* Read and write a word at address p */
// #define GET(p) (*(unsigned int *)(p))
// #define PUT(p, val) (*(unsigned int *)(p) = (val))
// /* Read the size and allocated fields from address p */
// #define GET_SIZE(p) (GET(p) & ~0x7)
// #define GET_ALLOC(p) (GET(p) & 0x1)
// /* Given block ptr bp, compute address of its header and footer */
// #define HDRP(bp) ((char *)(bp) - WSIZE)
// #define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
// /* Given block ptr bp, compute address of next and previous blocks */
// #define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) /* */
// #define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
// #define PREV_FREEP(bp)  (*(void **)(bp))                // 다음 가용 블록의 주소
// #define NEXT_FREEP(bp) (*(void **)(bp + WSIZE))         // 이전 가용 블록의 주소


// #define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// static char *heap_listp;
// static char *free_listp;

// static void place(void *bp, size_t asize);
// static void *extend_heap(size_t words);
// static void *coalesce(void *bp);
// static void *find_fit(size_t asize);
// void put_front_of_freelist(void *bp);
// void remove_in_freelist(void *bp);

// static void *coalesce(void *bp)
// {
//     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//     size_t size = GET_SIZE(HDRP(bp));

//     if (prev_alloc && next_alloc) {           /* Case 1 */
//         put_front_of_freelist(bp);
//         return bp;
//     }

//     else if (prev_alloc && !next_alloc) {       /* Case 2 */
//         remove_in_freelist(NEXT_BLKP(bp));
//         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
//         PUT(HDRP(bp), PACK(size, 0));
//         PUT(FTRP(bp), PACK(size, 0));
//     }
    
//     else if (!prev_alloc && next_alloc) {       /* Case 3 */
//         remove_in_freelist(PREV_BLKP(bp));
//         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
//         PUT(FTRP(bp), PACK(size, 0));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         bp = PREV_BLKP(bp);
//     }
//     else {                                      /* Case 4 */
//         remove_in_freelist(NEXT_BLKP(bp));
//         remove_in_freelist(PREV_BLKP(bp));
//         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//         bp = PREV_BLKP(bp);
//     }
//     put_front_of_freelist(bp);
//     return bp;
// }

// void remove_in_freelist(void *bp)
// {
//     if (bp == free_listp)
//     {
//         free_listp = NEXT_FREEP(bp);
//     }
//     if (PREV_FREEP(bp) != NULL)
//     {
//         NEXT_FREEP(PREV_FREEP(bp)) = NEXT_FREEP(bp);
//     }
//     if (NEXT_FREEP(bp) != NULL)
//     {
//         PREV_FREEP(NEXT_FREEP(bp)) = PREV_FREEP(bp);
//     }
// }

// void put_front_of_freelist(void *bp)
// {
//     NEXT_FREEP(bp) = free_listp;
//     PREV_FREEP(bp) = NULL;
//     PREV_FREEP(free_listp) = bp;
//     free_listp = bp; // bp가 첫번째 블럭이므로
// }

// static void *extend_heap(size_t words)
// {
//     char *bp;
//     size_t size;
//     /* Allocate an even number of words to maintain alignment */
//     size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
//     if ((long)(bp = mem_sbrk(size)) == -1)
//         return NULL;

//     /* Initialize free block header/footer and the epilogue header */
//     PUT(HDRP(bp), PACK(size, 0)); /* Free block header */
//     PUT(FTRP(bp), PACK(size, 0)); /* Free block footer */

//     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */
//     /* Coalesce if the previous block was free */
//     return coalesce(bp);
// }

// static void *find_fit(size_t asize)
// {
//     /* First-fit search */
//     void *bp;
//     for (bp = free_listp; bp!=NULL; bp = NEXT_FREEP(bp)) {
//         if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
//             return bp;
//         }
//     }
//     return NULL; /* No fit */
// }

// static void place(void *bp, size_t asize)
// {
//     size_t csize = GET_SIZE(HDRP(bp));
//     remove_in_freelist(bp);

//     if ((csize - asize) >= (2*DSIZE)) 
//     {
//         PUT(HDRP(bp), PACK(asize, 1));
//         PUT(FTRP(bp), PACK(asize, 1));
//         bp = NEXT_BLKP(bp);
//         PUT(HDRP(bp), PACK(csize-asize, 0));
//         PUT(FTRP(bp), PACK(csize-asize, 0));
//         put_front_of_freelist(bp);
//     }
//     else {
//         PUT(HDRP(bp), PACK(csize, 1));
//         PUT(FTRP(bp), PACK(csize, 1));
//     }

// }
// /* 
//  * mm_init - initialize the malloc package.
//  */

// int mm_init(void)
// {
// /* Create the initial empty heap */
// if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1)
//     return -1;

// PUT(heap_listp, 0); /* Alignment padding */
// PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
// PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
// PUT(heap_listp + (3*WSIZE), NULL);
// PUT(heap_listp + (4*WSIZE), NULL);
// PUT(heap_listp + (5*WSIZE), PACK(0, 1)); /* Epilogue header */
// heap_listp += (2*WSIZE);
// /* Extend the empty heap with a free block of CHUNKSIZE bytes */
// if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
//     return -1;
// free_listp = NULL;
// return 0;
// }

// /* 
//  * mm_malloc - Allocate a block by incrementing the brk pointer.
//  *     Always allocate a block whose size is a multiple of the alignment.
//  */
// void *mm_malloc(size_t size)
// {
//     size_t asize; /* Adjusted block size */
//     size_t extendsize; /* Amount to extend heap if no fit */
//     char *bp;
//     /* Ignore spurious requests */
//     if (size == 0)
//         return NULL;

//     /* Adjust block size to include overhead and alignment reqs. */
//     if (size <= DSIZE)
//         asize = 2*DSIZE;
//     else
//         asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
//     /* Search the free list for a fit */
//     if ((bp = find_fit(asize)) != NULL) {
//         place(bp, asize);
//         return bp;
//     }
//     /* No fit found. Get more memory and place the block */
//     extendsize = MAX(asize,CHUNKSIZE);
//     if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
//         return NULL;
//     place(bp, asize);
//     return bp;
// }


// /*
//  * mm_free - Freeing a block does nothing.
//  */
// void mm_free(void *bp)
// {
//     size_t size = GET_SIZE(HDRP(bp));

//     PUT(HDRP(bp), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));
//     coalesce(bp);
// }

// /*
//  * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
//  */
// void *mm_realloc(void *ptr, size_t size)
// {
//     void *oldptr = ptr;
//     void *newptr;
//     size_t copySize;
    
//     newptr = mm_malloc(size);
//     if (newptr == NULL)
//       return NULL;
//     copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
//     if (size < copySize)
//       copySize = size;
//     memcpy(newptr, oldptr, copySize);
//     mm_free(oldptr);
//     return newptr;
// }



/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "team 6",
    /* First member's full name */
    "Gyeongwon_Seo",
    /* First member's email address */
    "tjsxor50@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* 아래의 세개의 변수는 realloc에서 사용하는 매코로 변수임 */
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))



/* Basic constants and macros */
#define WSIZE   8           /* Word and header/footer size(bytes) */
#define DSIZE   16           /* Double word size (bytes) */
#define CHUNKSIZE (1<<12)   /* Extend heap by this amount (bytes)*/

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* pack a size and allocated bit into a word */
/* 사이즈와 할당 되었는지 여부를 비트로 표현한다. */
#define PACK(size, alloc)   ((size) | (alloc))
/* Read and write a word at address p */
#define GET(p)      (*(unsigned int*)(p)) // p가 가리키고 있는 값을 가져옴.
#define PUT(p, val) (*(unsigned int*)(p) = (val)) // p가 가리키고 있는 곳에 val이라는 값을 넣음.

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~0x7) // 비트 연산을 통해 p의 값을 읽어옴.(8이상이면서 8이상의 8의 배수만 반환함.)
#define GET_ALLOC(p)    (GET(p) & 0x1) // 비트 연산을 통해 할당되어 있는지 확인. 1이면 할당, 0이면 할당 안됨.

/* Givne block ptr bp, compute the address of its header and footer */
/* bp는 헤더 다음, payload 시작부분을 가리키고 있다. */
#define HDRP(bp)        ((char *)(bp) - WSIZE) // bp가 가리키고 있는 블록의 헤더를 가리키는 주소값을 반환한다.
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // 풋터를 가리키는 주소값을 반환한다.

/* Given block ptr bp, compute address of next and previous blocks */
/* implicit은 블록의 사이즈로 다음블록이나 이전 블록으로 */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // 다음 블록의 bp를 계산해서 반환해 준다. 현재 주소(bp) + 현재 블록의 헤더의 값(현재 블록의 사이즈)
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // 이전 블록의 bp를 계산해서 바환해 준다. 현재 주소(bp) - 이전 블록의 풋터의 값(이전 블록의 사이즈)

void *heap_listp;

/*
 * coalesce - 
 */
static void *coalesce(void *bp) // bp는 기준이 되는 블록의 포인터, 4가지 케이스로 나누어서 해당되는 케이스에서 연결을 해준다.
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); // 기준 블록 이전의 블록이 할당 되었는지 여부를 변수에 저장한다. 1은 할당, 0은 할당 안됨.
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); // 기준 블록 다음의 블록이 할당되었는지 여부를 변수에 저장.
    size_t size = GET_SIZE(HDRP(bp)); // 기준 블록의 사이즈

    if (prev_alloc && next_alloc) {         /* Case 1 */
        return bp;                          // 전후로 다 할당이 되어 있는 경우. 연결할 필요가 없으니 bp를 리턴해준다.
    }

    else if (prev_alloc && !next_alloc) {           /* Case 2 */ // 다음 블록은 할당 되지 않은 경우.
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));      // 현재 블록의 사이즈에 다음 블록의 사이즈를 더해준다.
        PUT(HDRP(bp), PACK(size, 0));               // 헤더를 업데이트! 사이즈와 가용 블록이라는 걸 업데이트 해준다.
        PUT(FTRP(bp), PACK(size, 0));               // 풋터를 업데이트!
    }

    else if(!prev_alloc && next_alloc) {    /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))); // 사이즈 구해주기!
        PUT(FTRP(bp), PACK(size, 0)); // 풋터 업데이트
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0)); // 헤더 업데이트, 이전 블록 헤더에 업데이트
        bp = PREV_BLKP(bp);
    }

    else{
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +     
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}
/* 
 * extend_heap - extends the heap with a new free block
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment*/
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; // 정렬 조건에 따라 2워드씩 할당하기 위한 코드
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));           /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));           /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   /* New epilogue header*/

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/*
 * find_fit
 */
static void *find_fit(size_t asize) // 힙리스트에서 할당할 사이즈에 맞는 가용 블록을 찾는다.
{
    /* First-fit search */
    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) { // 할당되지 않았고 할당할 블록 사이즈 보다 큰 가용 블록이 있으면
            return bp; 
        }
    }
    return NULL; /* No fit */ // 해당하는 가용 블록을 못 찾으면 NULL을 리턴
}

/*
 * place
 */
static void place(void *bp, size_t asize) // 할당할 블록의 포인터(bp)에 asize만큼 할당
{
    size_t csize = GET_SIZE(HDRP(bp)); // find_fit을 통해 찾은 가용 블록 사이즈

    if ((csize-asize) >= (2*DSIZE)) { // 할당하고 남은 가용 블록이 최소 가용 블록 사이즈(2*DSIZE)보다 크거나 같으면
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
    }
    else { // 남은 가용 블록이 최소 가용 블록 사이즈보다 작으면 너 다 해라.
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        /* Epilogue header */
    heap_listp += (2*WSIZE);

    /* Extent the empty heap wit a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;       /* Adjust block size */
    size_t extendsize;  /* Amount to extend heap if no fit */
    char *bp;

    /* IGnore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs */
    if (size <= DSIZE)
        asize = 2*DSIZE; // 헤더와 풋터가 1워드씩 더블워드, 정렬조건 때문에 데이터를 위해 더블워드 할당, 따라서 2*더블워드를 할당한다.
    else
        asize = DSIZE * ((size +(DSIZE) + (DSIZE-1)) / DSIZE); // else로 왔으면 size는 17이상이다. 따라서 asize는 3*DSIZE가 된다.

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) { // 가용 블록을 찾아서 bp를 업데이트
        place(bp, asize); // bp가 정해졌으면 bp에 asize만큼 할당
        return bp;
    }

    /* NO fit found. Get more memory and place the block */
    // 바로 위의 조건문이 false이면 find_fit으로 가용 블록을 못 찾았다는 의미이므로 힙을 늘려준다.
    extendsize = MAX(asize, CHUNKSIZE); // 힙을 늘려줄 기본단위인  CHUNKSIZE와 내가 할당할 asize 중 큰 사이즈로 힙을 늘려준다. 늘려줄 사이즈를 결정.
    if ((bp = extend_heap(extendsize/WSIZE))==NULL) // 만약 힙이 늘려지지 않으면 NULL을 리턴
        return NULL;
    place(bp, asize); // 힙을 늘리는게 성공했으면 할당
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp) // 해당 블록을 free하고 연결해준다.
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
