#pragma once

#include <immintrin.h>

namespace ip
{
	class SSE
	{
	public:
		static __forceinline __m128 operator+(__m128 m128a, __m128 m128b) { return _mm_add_ps(m128a, m128b); }
		static __forceinline __m128 operator-(__m128 m128a, __m128 m128b) { return _mm_sub_ps(m128a, m128b); }
		static __forceinline __m128 operator*(__m128 m128a, __m128 m128b) { return _mm_mul_ps(m128a, m128b); }
		static __forceinline __m128 operator/(__m128 m128a, __m128 m128b) { return _mm_div_ps(m128a, m128b); }

		static __forceinline __m128 operator+(__m128 m128a, float value) { return _mm_add_ps(m128a, _mm_set_ps1(value)); }
		static __forceinline __m128 operator-(__m128 m128a, float value) { return _mm_sub_ps(m128a, _mm_set_ps1(value)); }
		static __forceinline __m128 operator*(__m128 m128a, float value) { return _mm_mul_ps(m128a, _mm_set_ps1(value)); }
		static __forceinline __m128 operator/(__m128 m128a, float value) { return _mm_div_ps(m128a, _mm_set_ps1(value)); }

		static __forceinline __m128 operator&(__m128 m128a, __m128 m128b) { return _mm_and_ps(m128a, m128b); }
		static __forceinline __m128 operator|(__m128 m128a, __m128 m128b) { return _mm_or_ps(m128a, m128b); }

		static __forceinline __m128 operator<(__m128 m128a, __m128 m128b) { return _mm_cmplt_ps(m128a, m128b); }
		static __forceinline __m128 operator>(__m128 m128a, __m128 m128b) { return _mm_cmpgt_ps(m128a, m128b); }
		static __forceinline __m128 operator<=(__m128 m128a, __m128 m128b) { return _mm_cmple_ps(m128a, m128b); }
		static __forceinline __m128 operator>=(__m128 m128a, __m128 m128b) { return _mm_cmpge_ps(m128a, m128b); }
		static __forceinline __m128 operator==(__m128 m128a, __m128 m128b) { return _mm_cmpeq_ps(m128a, m128b); }
		static __forceinline __m128 operator!=(__m128 m128a, __m128 m128b) { return _mm_cmpneq_ps(m128a, m128b); }
	};

	class AVX
	{
		static __forceinline __m256d operator+(__m256d m256da, __m256d m256db) { return _mm256_add_pd(m256da, m256db); }
		static __forceinline __m256d operator-(__m256d m256da, __m256d m256db) { return _mm256_sub_pd(m256da, m256db); }
		static __forceinline __m256d operator*(__m256d m256da, __m256d m256db) { return _mm256_mul_pd(m256da, m256db); }
		static __forceinline __m256d operator/(__m256d m256da, __m256d m256db) { return _mm256_div_pd(m256da, m256db); }

		static __forceinline __m256d operator+(__m256d m256da, double value) { return _mm256_add_pd(m256da, _mm256_set1_pd(value)); }
		static __forceinline __m256d operator-(__m256d m256da, double value) { return _mm256_sub_pd(m256da, _mm256_set1_pd(value)); }
		static __forceinline __m256d operator*(__m256d m256da, double value) { return _mm256_mul_pd(m256da, _mm256_set1_pd(value)); }
		static __forceinline __m256d operator/(__m256d m256da, double value) { return _mm256_div_pd(m256da, _mm256_set1_pd(value)); }

		static __forceinline __m256d operator&(__m256d m256da, __m256d m256db) { return _mm256_and_pd(m256da, m256db); }
		static __forceinline __m256d operator|(__m256d m256da, __m256d m256db) { return _mm256_or_pd(m256da, m256db); }
	};
}
