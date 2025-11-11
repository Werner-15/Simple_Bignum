#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
/*
In this program we'll make our algorithm better,
especially for sub and divide.
*/

/*5<LSIZE<32767*/
#define LSIZE 200
/*The macro is used for Large2Str function.*/
#define LSTR_SIZE (LSIZE+2)


/**
 *	@brief	This structure defines an integer which can be extremely large.
 *  @detail	num[0]=Lowest digit,num[1]=2nd lowest,num[2]=3rd lowest,...
 * 
 * array index:	0,1,2,3,4,...,max
 * weigh:low->high
 * Unused high digits should be all 0.
 * 
 */
typedef struct LargeInt{
	uint8_t isNegative;
	uint8_t num[LSIZE];
	
} LargeInt;


typedef enum LargeInt_CheckLegal_Result{
	LargeInt_CheckLegal_Result_OK,
	LargeInt_CheckLegal_Result_Fail
} LargeInt_CheckLegal_Result;


/**
 * @brief This function returns a number divided by 10 quickly,
 * especially in CPUs without hardware division units,
 * but with hardware multiplication units like ARM Cortex-M3.
 * 
 * @param x :The number to process.
 * 
 * @return x/10
 */
static inline int8_t Div_10_m3(int8_t x){
	
	if(x>=0){
		return (x*13)>>7;
	}
	else{
		return (x==-128)?(-12):-(((-x)*13)>>7);
	}
	
}

/**
 * @brief This function returns a number divided by 10 quickly,
 * especially in CPUs with neither hardware division units nor
 * hardware multiplication units like Intel 8051.
 * 
 * @param x :The number to process.
 * 
 * @return x/10
 */
static inline int8_t Div_10_8051(int8_t x){
	int16_t y= x>=0?x:(-x),z=(y<<3)+(y<<2)+(y);
	
	if(x>=0){
		return (z)>>7;
	}
	else{
		return (x==-128)?(-12):-(z>>7);
	}
}


/**
 * @brief It helps to check if x is okay.
 * 
 * @param x:The num to identify.
 * 
 * @return @ref LargeInt_CheckLegal_Result
 */
LargeInt_CheckLegal_Result LargeInt_CheckLegal(LargeInt *x){
	if(x==NULL|| 
		(x->isNegative!=0 && x->isNegative!=1)){
		
		return LargeInt_CheckLegal_Result_Fail;
	}
	uint16_t i=0;
	for(i=0;i<LSIZE;i++){
		if(x->num[i]>=10){
			
			return LargeInt_CheckLegal_Result_Fail;
		}
	}
	return LargeInt_CheckLegal_Result_OK;
}

/**
 *@brief	This function turns a long long into a largeInt
 *@param	LargeInt *returnPlace: the place to return the output.
 *			You should input a variable's address as a parameter.
 *@param	int64_t input: the input long long int.
*/
void LargeInt_Long2Large(LargeInt *returnPlace,int64_t input){
	if(returnPlace==NULL){
		return;
	}
	
	LargeInt output;
	output.isNegative = (input<0)?1:0;
	/*i is here to decide the index.*/
	uint16_t i=0;
	uint64_t abs_input = input>=0 ? input:(-input);
	
	
	for(i=0;abs_input!=0;i++,abs_input/=10){
		output.num[i] = abs_input%10;
		
	}
	/*Fill the rest.*/
	for(;i<LSIZE;i++){
		output.num[i]=0;
	}
	
	/*Output.*/
	returnPlace->isNegative = output.isNegative;
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i] = output.num[i];
	}
	
}

typedef enum LargeInt_Large2Str_Results{
	LargeInt_Large2Str_Results_OK,
	LargeInt_Large2Str_Results_Param_Error
} LargeInt_Large2Str_Results;
/**
 * @brief This function turns a large int into a string.
 * @param	returnPlace:A char array,with its space up to LSIZE+2.
 * @param	x:The large int to convert.
 */
LargeInt_Large2Str_Results \
LargeInt_Large2Str(char *returnPlace,LargeInt *x){
	uint8_t front_0_flag=1;
	int16_t i=0,j,len;
	char ret[LSTR_SIZE];
	char *p=ret;
	
	
	if(LargeInt_CheckLegal(x)!=LargeInt_CheckLegal_Result_OK ||
		returnPlace==NULL){
		return LargeInt_Large2Str_Results_Param_Error;
	}
	
	front_0_flag=1;
	len=1;
	for(i=LSIZE-1;i>=0;i--){
		if(x->num[i]!=0){
			len=i+1;
			front_0_flag=0;
			break;
		}
	}
	
	/*x==0*/
	if(front_0_flag==1){
		returnPlace[0]='0';
		returnPlace[1]=0;
		return LargeInt_Large2Str_Results_OK;
	}
	
	/*x!=0*/
	if(x->isNegative==1){
		*p='-';
		p++;
		
	}
	
	for(i=len-1;i>=0;i--){
		//printf(":%d\n",x->num[i]);
		p[len-1-i]=x->num[i]+'0';
	}
	
	p[len]=0;
	
	for(i=0;ret[i]!=0;i++){
		returnPlace[i]=ret[i];
	}
	returnPlace[i]=0;
	
	return LargeInt_Large2Str_Results_OK;
	
}

typedef enum LargeInt_Str2LargeResults{
	LargeInt_Str2LargeResults_OK,
	LargeInt_Str2LargeResults_Param_Error,
	LargeInt_Str2LargeResults_Overflow
} LargeInt_Str2LargeResults;

LargeInt_Str2LargeResults \
LargeInt_Str2Large(LargeInt *returnPlace,char *s){
	int16_t i,len;
	LargeInt result;
	char *p=s;
	
	if(s==NULL||returnPlace==NULL){
		return LargeInt_Str2LargeResults_Param_Error;
	}
	for(i=0;s[i]!=0;i++){
		if((s[i]>'9'||s[i]<'0') && (!( (i==0)&&(s[i]=='-') )) ){
			return LargeInt_Str2LargeResults_Param_Error;
		}
		if(i>=LSIZE && s[i]!=0){
			return LargeInt_Str2LargeResults_Overflow;
		}
	}
	len = i;
	
	result.isNegative=0;
	if(s[0]=='-'){
		result.isNegative=1;
		len--;
		p++;
	}
	
	for(i=0;i<=len-1;i++){
		result.num[len-i-1] = p[i]-'0';
		
	}
	for(i=len;i<LSIZE;i++){
		result.num[i]=0;
	}
	
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i]=result.num[i];
	}
	returnPlace->isNegative=result.isNegative;
	
	return LargeInt_Str2LargeResults_OK;
	
}

/**
 * @defgroup	LargeInt_Compare_Results
 * @brief	The type is used as return values of compare function.
 * 			
 */
typedef enum LargeInt_Compare_Results{
	LargeInt_Compare_Results_SAME=0,
	LargeInt_Compare_Results_A_BIG=1,
	LargeInt_Compare_Results_A_SMALL=-1,
	LargeInt_Compare_Results_Error_Param=2,
	LargeInt_Compare_Results_Error_Conflict
} LargeInt_Compare_Results;

/**
 *@brief	It helps compare the two large ints without changing either.
 *@param	LargeInt *a,LargeInt *b:Two large ints to compare.
 *@return	LargeInt_Compare_Results:see @ref	LargeInt_Compare_Results.
 */
LargeInt_Compare_Results LargeInt_Compare(LargeInt *a,LargeInt *b){
	int16_t i = LSIZE-1;
	uint8_t reverse_result = 0;
	LargeInt_Compare_Results res;
	
	
	if(LargeInt_CheckLegal(a)==LargeInt_CheckLegal_Result_Fail ||
	LargeInt_CheckLegal(b)==LargeInt_CheckLegal_Result_Fail){
		
		return LargeInt_Compare_Results_Error_Param;
	}
	
	/*Compare the sign.*/
	if(a->isNegative==1){
		if(b->isNegative==0){
			return LargeInt_Compare_Results_A_SMALL;
		}
		else{
			reverse_result=1;
		}
	}
	else{
		if(b->isNegative==1){
			return LargeInt_Compare_Results_A_BIG;
		}
	}
	
	res = LargeInt_Compare_Results_SAME;
	/*Compare each digit.*/
	for(i = LSIZE-1;i>=0;i--){
		if(a->num[i]!=b->num[i]){
			
			if(a->num[i]>b->num[i]){
				res = LargeInt_Compare_Results_A_BIG;
			}
			else{
				res = LargeInt_Compare_Results_A_SMALL;
			}
			break;
		}
		
	}
	
	
	
	/*Reverse.*/
	if(reverse_result==1){
		switch(res){
		case LargeInt_Compare_Results_A_BIG:
			res = LargeInt_Compare_Results_A_SMALL;
			break;
		case LargeInt_Compare_Results_A_SMALL:
			res = LargeInt_Compare_Results_A_BIG;
			break;
		case LargeInt_Compare_Results_SAME:
			break;
		default:
			res = LargeInt_Compare_Results_Error_Conflict;
			break;
		}
	}
	return res;
}


/**
 * @brief This function compares the absolute value of two large ints.
 * 
 * @param a 
 * @param b 
 * 
 * @return @ref LargeInt_Compare_Results.
 */
LargeInt_Compare_Results LargeInt_Compare_ABS(LargeInt *a,LargeInt *b){
	
	LargeInt_Compare_Results res;
	int16_t i;
	
	if(LargeInt_CheckLegal(a)==LargeInt_CheckLegal_Result_Fail ||
		LargeInt_CheckLegal(b)==LargeInt_CheckLegal_Result_Fail){
		
		return LargeInt_Compare_Results_Error_Param;
	}
	
	res = LargeInt_Compare_Results_SAME;
	for(i=LSIZE-1;i>=0;i--){
		if(a->num[i]!=b->num[i]){
			if(a->num[i]>b->num[i]){
				res = LargeInt_Compare_Results_A_BIG;
			}
			else if(a->num[i]<b->num[i]){
				res=LargeInt_Compare_Results_A_SMALL;
			}
			else{
				res=LargeInt_Compare_Results_Error_Conflict;
			}
			break;
		}
	}
	return res;
	
}

typedef enum LargeInt_Add_Results{
	LargeInt_Add_Results_OK,
	LargeInt_Add_Results_Error_Param,
	LargeInt_Add_Results_Overflow,
	LargeInt_Add_Results_Program_Error
} LargeInt_Add_Results;

/**
 *@brief	This function ignores the two numbers' sign,
 *			only adding their numbers together.
 *@param	a,b:The two LargeInts to add.
 *@param 	returnPlace:The place to return.
 */
static LargeInt_Add_Results LargeInt_Add_Unsigned(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	int16_t i=0;
	uint8_t carry = 0;
	
	/*Check.*/
	if(LargeInt_CheckLegal(a)==LargeInt_CheckLegal_Result_Fail ||
	LargeInt_CheckLegal(b)==LargeInt_CheckLegal_Result_Fail	||
	returnPlace==NULL){
		return LargeInt_Add_Results_Error_Param;
	}
	
	/*Default sign.*/
	returnPlace->isNegative = 0;
	
	/*Add the digits.*/
	carry = 0;
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i] = a->num[i] + b->num[i] + carry;
		carry = returnPlace->num[i]/10;
		returnPlace->num[i] %= 10;
	}
	
	
	/*Return.*/
	if(carry!=0){
		return LargeInt_Add_Results_Overflow;
	}
	return LargeInt_Add_Results_OK;
}



typedef enum LargeInt_Sub_Results{
	LargeInt_Sub_Results_OK,
	LargeInt_Sub_Results_Error_Param,
	LargeInt_Sub_Results_Underflow,
	LargeInt_Sub_Results_Program_Error
} LargeInt_Sub_Results;

/**
 * @brief Unsigned subtraction.
 * @param returnPlace the place to return.
 * @param a Number to minus.
 * @param b The sub value.
 * @note 1. ASSERT a>=b !!!
 *       2. It also ignores the sign.
 */
static LargeInt_Sub_Results LargeInt_Sub_Unsigned(LargeInt *returnPlace, LargeInt *a, LargeInt *b) {
	int16_t i = 0;
	uint8_t borrow = 0;
	/*Temporary difference.*/
	int16_t diff=0;
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Sub_Results_Error_Param;
	}
	
	/*Default sign.*/
	returnPlace->isNegative = 0;
	
	/* Minus each digit.*/
	for (i = 0; i < LSIZE; i++) {
		diff = (int16_t)a->num[i] - (int16_t)b->num[i] - (int16_t)borrow;
		borrow = 0;
		
		while(diff<0) {
			diff+=10;
			borrow++;
		}
		
		/*Output.*/
		returnPlace->num[i] = (uint8_t)diff;
	}
	
	/*Return.*/
	if (borrow != 0) {
		return LargeInt_Sub_Results_Underflow;
	}
	
	return LargeInt_Sub_Results_OK;
}


/**
 * @brief It adds a and b together,considering the sign.
 * 
 * @param a Number a.
 * @param b Number b.
 * @param returnPlace	The place to return.
 * @param neg: neg==0->perform a+b,otherwise perform a-b.
 * 
 * @return LargeInt_Add_Results the result.
 */
static LargeInt_Add_Results 
LargeInt_neg_Add(LargeInt *returnPlace,LargeInt *a,LargeInt *b,uint8_t neg){
	LargeInt_Add_Results st;
	LargeInt_Sub_Results sst;
	LargeInt_Compare_Results com;
	//LargeInt temp;
	uint16_t i=0;
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		
		return LargeInt_Add_Results_Error_Param;
	}
	
	/*neg is a boolean value.*/
	neg=(neg==0) ? 0x00 : 0x01;
	
	/*Consider the 4 conditions.*/
	if(a->isNegative==0){
		
		
		/*
		WARNING:
		x&0x01==0 will calculate x&(0x01==0)(Wrong!!!)
		So we should write:
		(x&0x01)==0
		BE CAREFUL!!!
		*/
		
		
		if((((b->isNegative)^neg)&0x01)==0){
			
			st=LargeInt_Add_Unsigned(returnPlace,a,b);
			returnPlace->isNegative=0;
			return st;
		}
		else if(((b->isNegative^neg)&0x01)==1){
			com=LargeInt_Compare_ABS(a,b);
			
			/*Act according to different sizes of a and b.*/
			switch(com){
			case LargeInt_Compare_Results_A_BIG:
				/*a>b,a-abs(b)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,a,b);
				returnPlace->isNegative=0;
				break;
			case LargeInt_Compare_Results_A_SMALL:
				/*a<b,a-abs(b)=-(abs(b)-a)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,b,a);
				returnPlace->isNegative=1;
				break;
			case LargeInt_Compare_Results_SAME:
				for(i=0;i<LSIZE;i++){
					returnPlace->num[i]=0;
				}
				returnPlace->isNegative=0;
				sst = LargeInt_Sub_Results_OK;
				break;
			default:
				
				return LargeInt_Add_Results_Program_Error;
			}
			
			/*Deal with the state.*/
			switch(sst){
			case LargeInt_Sub_Results_OK:
				return LargeInt_Add_Results_OK;
			case LargeInt_Sub_Results_Underflow:
				return LargeInt_Add_Results_Overflow;
			case LargeInt_Sub_Results_Error_Param:
				return LargeInt_Add_Results_Error_Param;
			case LargeInt_Sub_Results_Program_Error:
				return LargeInt_Add_Results_Program_Error;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			return LargeInt_Add_Results_Program_Error;
			
		}
		else{
			
			return LargeInt_Add_Results_Error_Param;
		}
	}
	
	else if(a->isNegative==1){
		if(((b->isNegative^neg)&0x01)==1){
			st=LargeInt_Add_Unsigned(returnPlace,a,b);
			returnPlace->isNegative=1;
			return st;
		}
		else if(((b->isNegative^neg)&0x01)==0){
			
			
			com=LargeInt_Compare_ABS(a,b);
			
			
			/*Act according to different sizes of a and b.*/
			switch(com){
			case LargeInt_Compare_Results_A_SMALL:
				/*a<b,b-abs(a)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,b,a);
				returnPlace->isNegative=0;
				break;
			case LargeInt_Compare_Results_A_BIG:
				/*a>b,b-abs(a) = -(abs(a)-b)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,a,b);
				returnPlace->isNegative=1;
				break;
			case LargeInt_Compare_Results_SAME:
				for(i=0;i<LSIZE;i++){
					returnPlace->num[i]=0;
				}
				returnPlace->isNegative=0;
				sst = LargeInt_Sub_Results_OK;
				break;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			
			/*Deal with the state.*/
			switch(sst){
			case LargeInt_Sub_Results_OK:
				return LargeInt_Add_Results_OK;
			case LargeInt_Sub_Results_Underflow:
				return LargeInt_Add_Results_Overflow;
			case LargeInt_Sub_Results_Error_Param:
				return LargeInt_Add_Results_Error_Param;
			case LargeInt_Sub_Results_Program_Error:
				return LargeInt_Add_Results_Program_Error;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			return LargeInt_Add_Results_Program_Error;
			
		}
		else{
			return LargeInt_Add_Results_Error_Param;
		}
	}
	else{
		return LargeInt_Add_Results_Error_Param;
	}
	
	
}

LargeInt_Add_Results LargeInt_Add(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	return LargeInt_neg_Add(returnPlace,a,b,0);
}



/**
 * @brief It calculates a-b,considering the sign.
 * 
 * @param a Number a.
 * @param b Number b.
 * @param returnPlace	The place to return. 
 * 
 * @return LargeInt_Add_Results the result.
 */
LargeInt_Sub_Results LargeInt_Sub(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt_Sub_Results ret;
	LargeInt_Add_Results ad;
	//LargeInt neg_b;
	//uint16_t i;
	
	
	
	ad=LargeInt_neg_Add(returnPlace,a,b,1);
	
	/*Deal with the state.*/
	switch(ad){
	case LargeInt_Add_Results_OK:
		return LargeInt_Sub_Results_OK;
	case LargeInt_Add_Results_Overflow:
		return LargeInt_Sub_Results_Underflow;
	case LargeInt_Add_Results_Error_Param:
		return LargeInt_Sub_Results_Error_Param;
	case LargeInt_Add_Results_Program_Error:
		return LargeInt_Sub_Results_Program_Error;
	default:
		return LargeInt_Sub_Results_Program_Error;
	}
	return LargeInt_Sub_Results_Program_Error;
	
}




typedef enum LargeInt_Multiply_Results{
	LargeInt_Multiply_Results_OK,
	LargeInt_Multiply_Results_Error_Param,
	LargeInt_Multiply_Results_Overflow,
	LargeInt_Multiply_Results_Program_Error
} LargeInt_Multiply_Results;


/**
 *@brief	This function tries to multiply two large numbers,considering signs.
 *@param	returnPlace:The place to return the results.
 *@param	a,b:The numbers to multiply.
 *@return	LargeInt_Multiply_Results.
*/
LargeInt_Multiply_Results  \
LargeInt_Multiply(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt result;
	uint16_t alen=0,blen=0;
	int16_t i,j;
	
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Multiply_Results_Error_Param;
	}
	
	
	
	/*Count how many digits there are in each number.*/
	alen=1,blen=1;
	for(i=LSIZE-1;i>=0;i--){
		if(a->num[i]!=0){
			alen=i+1;
			break;
		}
	}
	for(i=LSIZE-1;i>=0;i--){
		if(b->num[i]!=0){
			blen=i+1;
			break;
		}
	}
	
	
	/*For calculation a(m digits) times b(n digits),
	the result's number of digits should be 
	either m+n-1 or m+n.
	This can be used as a proof of overflow.*/
	if(alen+blen-1>LSIZE){
		return LargeInt_Multiply_Results_Overflow;
	}
	
	/*For convenience,we should make sure alen>=blen,
	otherwise we swap the two numbers.*/
	if(alen<blen){
		return LargeInt_Multiply(returnPlace,b,a);
	}
	
	/*
	Now judge the sign.
	sign equal ->0,
	different ->1,
	consider XOR calculation.
	*/
	result.isNegative = a->isNegative ^ b->isNegative;
	result.isNegative &=0x01;/*Clear up other bits.*/
	
	
	
	/*Clear result bar.*/
	for(i=0;i<LSIZE;i++){
		result.num[i]=0;
	}
	
	/*
	Now operate the two absolute values' result,
	which is the main part.
	*/
	i=0;j=0;
	for(j=0;j<blen;j++){
		if(b->num[j]==0){
			continue;
		}
		
		for(i=0;i<alen;i++){
			result.num[i+j] += (a->num[i]) * (b->num[j]);
		}
		for(i=0;i<j+alen;i++){
			if(i<LSIZE-1){
				result.num[i+1]+=result.num[i]/10;
			}
			else{
				if(result.num[i]>=10){
					return LargeInt_Multiply_Results_Overflow;
				}
			}
			result.num[i]%=10;
		}
	}
	
	/*return.*/
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i] = result.num[i];
	}
	returnPlace->isNegative = result.isNegative;
	
	return LargeInt_Multiply_Results_OK;
	
	
}






typedef enum LargeInt_Divide_Results{
	LargeInt_Divide_Results_OK,
	LargeInt_Divide_Results_Error_Param,
	LargeInt_Divide_Results_Overflow,
	LargeInt_Divide_Results_Program_Error,
	LargeInt_Divide_Results_Error_Div_0
} LargeInt_Divide_Results;



/**
 * @brief	This function performs a/b,both of which are LargeInts.
 * @param	LargeInt *returnPlace,LargeInt *a,LargeInt *b
 * @note	b!=0
 */
LargeInt_Divide_Results  \
LargeInt_Divide(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt divisor,remainder,result,buffer;
	uint16_t alen,blen,rlen;
	int16_t i,j,k,l,p,ori_p;
	/*Try to get the results with binary search.*/
	uint16_t t0,t1,t;
	/*A flag.*/
	uint8_t escape = 0;
	LargeInt_Compare_Results x;
	
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Divide_Results_Error_Param;
	}
	
	/*Div 0 check.*/
	j=1;
	for(i=0;i<LSIZE;i++){
		if(b->num[i]!=0){
			j=0;
			break;
		}
	}
	if(j==1){
		return LargeInt_Divide_Results_Error_Div_0;
	}
	
	/*Count how many digits there are in each number.*/
	alen=1,blen=1;
	for(i=LSIZE-1;i>=0;i--){
		if(a->num[i]!=0){
			alen=i+1;
			break;
		}
	}
	for(i=LSIZE-1;i>=0;i--){
		if(b->num[i]!=0){
			blen=i+1;
			break;
		}
	}
	
	/*
	Now judge the sign.
	sign equal ->0,
	different ->1,
	consider XOR calculation.
	*/
	result.isNegative = a->isNegative ^ b->isNegative;
	result.isNegative &=0x01;/*Clear up other bits.*/
	
	/*Clear result bar and remainder bar,
	set divisor(abs(b)) and dividend(abs(a)).*/
	for(i=0;i<LSIZE;i++){
		result.num[i]=0;
		remainder.num[i]=0;
		divisor.num[i]=b->num[i];
	}
	
	divisor.isNegative=0;
	buffer.isNegative=0;
	/*Get rid of special conditions.*/
	x=LargeInt_Compare_ABS(a,&divisor);
	switch (x) {
	case LargeInt_Compare_Results_A_SMALL:
		returnPlace->isNegative=0;
		for(i=0;i<LSIZE;i++){
			returnPlace->num[i]=0;
		}
		return LargeInt_Divide_Results_OK;
		break;
	case LargeInt_Compare_Results_SAME:
		returnPlace->num[0]=1;
		for(i=1;i<LSIZE;i++){
			returnPlace->num[i]=0;
		}
		return LargeInt_Divide_Results_OK;
		break;
	case LargeInt_Compare_Results_A_BIG:
		break;
	default:
		
		return LargeInt_Divide_Results_Program_Error;
		break;
	}
	
	remainder.isNegative=0;
	divisor.isNegative=0;
	/*Now is the state machine part.*/
	/*j works as the dividend ptr.*/
	j=alen-1;
	/*p is used as result digit indicator.*/
	ori_p=(blen>1)?(alen-blen+1):(alen-1);
	p=ori_p;
	while(1){
		
		/*Remainder length.*/
		rlen=1;
		for(i=LSIZE-1;i>=0;i--){
			
			if(remainder.num[i]!=0){
				rlen=i+1;
				break;
			}
		}
		
		if(rlen!=blen){
			/*
			Move remainder to the higher level.
			It means:
			b=12345
			r=01344
			=>r=13440
			*/
			i=1;
			/*Move digits.*/
			for(i=1;i<=rlen;i++){
				remainder.num[blen-i]=remainder.num[rlen-i];
			}
			for(i=0; i < (blen - rlen); i++){
				remainder.num[i] = 0;
			}
			
			/*Copy the rest digits.*/
			i=j;
			i-=(blen-rlen-1);
			if(i<0){
				i=0;
			}
			for(k=i,l=0 ; k<=j&&l<blen-rlen ; k++,l++){
				remainder.num[l]=a->num[k];
			}
				
		}
		else{
			for(k=LSIZE-2;k>=0;k--){
				remainder.num[k+1]=remainder.num[k];
			}
			i=j;
			remainder.num[0]=a->num[i];
			
			
		}
		/*
		printf("%d,%d,%d,%d,%d!%d\n",
			remainder.num[4],
			remainder.num[3],
			remainder.num[2],
			remainder.num[1],
			remainder.num[0],p);
		*/
		/*Start dividing.*/
		if(p==ori_p){
			while(1){
				
				x=LargeInt_Compare(&remainder,&divisor);
				if(x==LargeInt_Compare_Results_Error_Conflict ||
					x==LargeInt_Compare_Results_Error_Param){
					
					return LargeInt_Divide_Results_Program_Error;
				}
				else if(x==LargeInt_Compare_Results_A_SMALL){
					
					break;
				}
				else if(x==LargeInt_Compare_Results_A_BIG ||
					x==LargeInt_Compare_Results_SAME){
					
					LargeInt_Sub(&buffer,&remainder,&divisor);
					
					for(k=0;k<LSIZE;k++){
						remainder.num[k]=buffer.num[k];
					}
					result.num[p]++;
					
				}
			}
		}
		else{
			x=LargeInt_Compare(&remainder,&divisor);
			if(x==LargeInt_Compare_Results_Error_Conflict ||
				x==LargeInt_Compare_Results_Error_Param){
				return LargeInt_Divide_Results_Program_Error;
			}
			else if(x==LargeInt_Compare_Results_A_SMALL){
				result.num[p]=0;
			}
			else{
				/*Binary search.*/
				t1=9;
				t0=0;
				
				escape=0;
				
				
				
				while(1){
					t=(t1+t0)>>1;
					for(l=0;l<LSIZE;l++){
						buffer.num[l]=0;
					}
					/*Perform divisor*t. */
					for(l=0;l<LSIZE;l++){
						buffer.num[l]=divisor.num[l]*t;
					}
					for(l=0;l<LSIZE;l++){
						buffer.num[l+1]+=buffer.num[l]/10;
						buffer.num[l]%=10;
					}
					
					/*
					printf("%d,%d,%d,%d,%d!%d,%d,%d\n",
					buffer.num[4],
					buffer.num[3],
					buffer.num[2],
					buffer.num[1],
					buffer.num[0],t1,t,t0);
					*/
					
					x=LargeInt_Compare(&buffer,&remainder);
					switch(x){
					case LargeInt_Compare_Results_A_BIG:
						t1=t-1;
						break;
					case LargeInt_Compare_Results_A_SMALL:
						t0=t+1;
						
						break;
					case LargeInt_Compare_Results_SAME:
						result.num[p]=t;
						escape=1;
						break;
					default:
						
						return LargeInt_Divide_Results_Program_Error;
					}
					
					if(t0>t1){
						result.num[p]=t1;
						escape=1;
					}
					
					
					if(escape==1){
						for(l=0;l<LSIZE;l++){
							buffer.num[l]=0;
						}
						/*Perform divisor*t. */
						for(l=0;l<LSIZE;l++){
							buffer.num[l]=
							divisor.num[l]*result.num[p];
						}
						for(l=0;l<LSIZE;l++){
							buffer.num[l+1]+=buffer.num[l]/10;
							buffer.num[l]%=10;
						}
						LargeInt_Sub(&remainder,&remainder,&buffer);
						break;
					}
					
				}
			}
			
			
		}
		
		
		
		j=i-1;
		p--;
		
		if(p<0||p>=LSIZE){
			returnPlace->isNegative = result.isNegative;
			for(i=0;i<LSIZE;i++){
				returnPlace->num[i] = result.num[i];
			}
			return LargeInt_Divide_Results_OK;
		}
		
		
	}
	
}

/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/

#include <time.h>
int main(){
	int64_t l = 16384299792458;
	int64_t m = -1145141919810;
	
	uint32_t i = 0,j;
	LargeInt x,y,z;
	
	time_t t1,t2;
	
	char ls0[LSTR_SIZE],ls1[LSTR_SIZE],ls2[LSTR_SIZE];
	
	LargeInt_Str2Large(&x,"001232");
	LargeInt_Large2Str(ls0,&x);
	printf("%s\n",ls0);
	
	for(i=-128;i<127;i++){
		if(Div_10_8051(i)!=i/10){
			printf("Wrong%d\n",i);
		}
	}
	
	LargeInt_Multiply_Results times_res;
	LargeInt_Divide_Results div_res;
	
	LargeInt_Long2Large(&x,l);
	LargeInt_Long2Large(&y,m);
	
	LargeInt_Large2Str(ls0,&x);
	printf("%s\n",ls0);
	
	printf("%lld\n",l);
	
	
	printf("%d\n",LargeInt_Compare(&x,&y));
	
	printf("Add ERR:%d\n",LargeInt_Add(&z,&x,&y));
	
	LargeInt_Large2Str(ls0,&z);
	LargeInt_Large2Str(ls1,&x);
	LargeInt_Large2Str(ls2,&y);
	printf("%s+%s=%s\n",ls1,ls2,ls0);
	printf("\n");
	
	LargeInt_Sub(&z,&x,&y);
	
	LargeInt_Large2Str(ls0,&z);
	LargeInt_Large2Str(ls1,&x);
	LargeInt_Large2Str(ls2,&y);
	printf("%s-%s=%s\n",ls1,ls2,ls0);
	printf("\n");
	
	
	
	times_res=LargeInt_Multiply(&z,&x,&y);
	LargeInt_Large2Str(ls0,&z);
	LargeInt_Large2Str(ls1,&x);
	LargeInt_Large2Str(ls2,&y);
	printf("%s*%s=%s\n",ls1,ls2,ls0);
	printf("TimesERR Code:%d\n",times_res);
	
	div_res=LargeInt_Divide(&z,&x,&y);
	LargeInt_Large2Str(ls0,&z);
	LargeInt_Large2Str(ls1,&x);
	LargeInt_Large2Str(ls2,&y);
	printf("%s/%s=%s\n",ls1,ls2,ls0);
	printf("DivideERR Code:%d\n",div_res);
	
	LargeInt_Multiply(&x,&x,&x);
	t1=clock();
	for(i=1;i<800;i++){
		for(j=1;j<1000;j++){
			LargeInt_Multiply(&z,&x,&y);
		}
	}
	t2=clock();
	printf("time: %d ms\n",t2-t1);
	
	return 0;
}
