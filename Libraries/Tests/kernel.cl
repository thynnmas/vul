__kerlen void main( __global float *a, __global float *c )
{
	uint id = get_global_id(0);
	c[ id ] = a[ id ] * a[ id ];
}
