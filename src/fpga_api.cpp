#include "fpga_api.h"
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define DATA_SIZE SIZE*(SIZE+1)*sizeof(float) // fpga bram data size

#define min(x,y) (((x)<(y))?(x):(y))

FPGA::FPGA(off_t data_addr, off_t api_addr)
{
    fd_ = open("/dev/mem", O_RDWR);
    data_ = static_cast<float*>(mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_, data_addr));
    api_ = static_cast<unsigned int*>(mmap(NULL, sizeof(unsigned int), PROT_READ|PROT_WRITE, MAP_SHARED,fd_, api_addr));
}

FPGA::~FPGA()
{
    munmap(data_, DATA_SIZE );
    munmap(api_, sizeof(unsigned int));
    close(fd_);
}

float* FPGA::matrix(void)
{
	return data_ + SIZE;
}

float* FPGA::vector(void)
{
	return data_;
}

const float* __attribute__((optimize("O0"))) FPGA::run()
{
    *api_ = 0x5555;
    while(*api_ == 0x5555);

    return data_;
} 

void FPGA::largeMV(const float* large_mat, const float* input,
		float* output, int N, int M)
{
	int i,j,k,p,q,r;

	float* vec = this->vector();
	float* mat = this->matrix();
	
	for(r=0; r < M; r++){
		output[r] = 0;
	}

	
for (j=0; j< ((M + SIZE -1) / SIZE); j++){

    for(i=0; i< ((N + SIZE -1) / SIZE); i++) {

	int mr = min(SIZE, M-SIZE*j);
	int nr = min(SIZE, N-SIZE*i);


	for (k=0; k< nr ; k++) {
                vec[k] = input[SIZE*i + k];
            }
	for (k= nr ; k < SIZE ; k++){
                vec[k] = 0;
            }

	for (p=0; p< mr; p++) for (q=0; q< nr; q++){
		mat[p*SIZE + q] = large_mat[ (j*SIZE + p)*N + i*SIZE + q ];
        }

	for (p=mr; p< SIZE; p++) for (q=0; q< nr; q++){
		mat[p*SIZE + q] = 0;
        }

   
	const float* temp_run =  this->run();

	for(r=0; r< mr; r++){
		output[j*SIZE +r] += temp_run[r];
        }

	
 		

    }

	

}

}
