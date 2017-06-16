#include "fpga_api.h"
#include <cstring>
#define DATA_SIZE SIZE*(SIZE+1) // fpga bram data size

#define min(x,y) (((x)<(y))?(x):(y))

FPGA::FPGA(off_t data_addr, off_t api_addr)
{
    api_ = new unsigned int[SIZE];    // use api_ as tempolar output 
    data_ = new float[DATA_SIZE];	
}

FPGA::~FPGA()
{
    delete[] api_;
    delete[] data_;
}

float* FPGA::matrix(void)
{
	return data_ + SIZE;
}

float* FPGA::vector(void)
{
	return data_;
}

const float* FPGA::run()
{
    float* vec = this->vector();
    float* mat = this->matrix();
    float* out  = reinterpret_cast<float*>(api_);  

    for(int i = 0 ; i < SIZE; ++i)
    {
        out[i] = 0;

        for(int j = 0 ; j < SIZE; ++j)
           out[i] += vec[j] * mat[SIZE*i + j];
    }

	for(int i = 0 ; i < SIZE; ++i)
	{
		data_[i] = out[i];
	}

    return data_;    
}

void FPGA::largeMV(const float* large_mat, const float* input,
		float* output, int N, int M)
{
	int i,j,k,p,q,r;

	float* vec = this->vector();
	float* mat = this->matrix();
	
	float* temp_out = new float[SIZE];
	const float* temp_run = new float[SIZE*(SIZE+1)];

for (r=0; r<SIZE; r++) temp_out[r] = 0;

	
for (j=0; j< ((M + SIZE -1) / SIZE); j++){

    for(i=0; i< ((N + SIZE -1) / SIZE); i++) {

	
	if(i == ((N + SIZE -1) / SIZE) -1 ) {
	    for (k=0; k< (N % SIZE) ; k++) vec[k] = input[SIZE*i + k];
	    for (k= (N % SIZE) ; k < SIZE ; k++) vec[k] = 0;
	}
	else {
	    for (k=0; k<SIZE; k++) {
		vec[k] = input[SIZE*i + k];
	    }
	}
	

	if(j == ((M + SIZE -1) / SIZE) -1 ) {

	    if(i == ((N + SIZE -1) / SIZE) -1 ) {

	      for (p=0; p< M % SIZE; p++)
		for (q=0; q< N % SIZE; q++)
		    mat[p*SIZE + q] = large_mat[ (j*SIZE + p)*N + i*SIZE + q ];

	      for (p= M%SIZE; p<SIZE; p++) 
		for (q=0; q< N % SIZE; q++) 
		    mat[p*SIZE + q] = 0;

	    }
	    else {
	        for (p=0; p< M % SIZE; p++) for (q=0; q<SIZE; q++) 
		    mat[p*SIZE + q] = large_mat[ (j*SIZE + p)*N + i*SIZE + q ];

	        for (p= M%SIZE; p<SIZE; p++) for (q=0; q<SIZE; q++) 
		    mat[p*SIZE + q] = 0;
	    }

	}
	else{

	    if (i == ((N + SIZE -1) / SIZE) -1 ) {
		for (p=0; p<SIZE; p++) for (q=0; q< N % SIZE; q++)
                mat[p*SIZE + q] = large_mat[ (j*SIZE + p)*N + i*SIZE + q ];
	    }
	    else {
		for (int p=0; p<SIZE; p++) for (int q=0; q<SIZE; q++)
                mat[p*SIZE + q] = large_mat[ (j*SIZE + p)*N + i*SIZE + q ];
	    }
	}
   
	temp_run =  (this->run()) ;
	
 		
	for(r=0; r<SIZE; r++)
	    temp_out[r] += temp_run[r];

    }
    if (j < (M+SIZE-1)/SIZE ) {
	for(r=0; r< SIZE; r++){
	    output[j*SIZE +r] = temp_out[r];

            temp_out[r]=0;
        }

    }	
    else
        for(r=0; r< M%SIZE; r++){
	    output[j*SIZE +r] = temp_out[r];

	    temp_out[r]=0;
        }
	

}


}



