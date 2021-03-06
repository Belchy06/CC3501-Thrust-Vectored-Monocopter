/*
 * PositionEstimator.c
 *
 *  Created on: 30 Sep 2021
 *      Author: Will
 */

#include "PositionEstimator.h"

void initializePositionEstimator(PositionEstimator *self) {
	for (uint8_t i = 0; i < 4; i++) {
		self->x[i] = 0.0f;
	}
	for (uint8_t i = 0; i < 16; i++) {
		self->P[i] = 0.0f;
	}
	self->estimatePosition= &estimatePosition;
	self->getPositionEstimate = &getPositionEstimate;
}

void estimatePosition(PositionEstimator *self, const float measurements[4],
		const float delta_t) {
	float F[16];
	float a21;
	float s;
	float x_pred[4];
	int i;
	float b_F[16];
	float a22;
	int i1;
	int jA;
	signed char i2;
	static const signed char a[8] = { 0, 0, 0, 0, 1, 0, 0, 1 };

	signed char i3;
	int j;
	static const signed char b_a[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
			0, 0, 1 };

	static const float dv[16] = { 0.001, 0.0, 0.0, 0.0,
								  0.0, 0.001, 0.0, 0.0,
								  0.0, 0.0, 0.1, 0.0,
								  0.0, 0.0, 0.0, 0.1 };

	float S[4];
	int mmj_tmp;
	signed char i4;
	int r1;
	float c_a[8];
	int r2;
	int jj;
	static const signed char iv[8] = { 0, 0, 1, 0, 0, 0, 0, 1 };

	int jp1j;
	int K_tmp;
	signed char ipiv[4];
	int ix;
	float K[16];
	float b_K[8];
	static const float dv1[4] = { 0.00024841137121251269, 0.0, 0.0,
			3.2967029712837327E-6 };

	static const signed char iv1[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
			0, 0, 1 };

	static const float dv2[16] = { 0.45403684594609556, 0.0, 0.0, 0.0, 0.0,
			0.2484429559918076, 0.0, 0.0, 0.0, 0.0, 0.00024841137121251269, 0.0,
			0.0, 0.0, 0.0, 3.2967029712837327E-6 };

	float b_measurements[2];
	F[0] = 1.0;
	F[4] = 0.0;
	a21 = delta_t * self->x[2];
	F[8] = a21;
	F[12] = 0.0;
	F[1] = 0.0;
	F[5] = 1.0;
	F[9] = 0.0;
	s = delta_t * self->x[3];
	F[13] = s;
	F[2] = 0.0;
	F[3] = 0.0;
	F[6] = 0.0;
	F[7] = 0.0;
	F[10] = 1.0;
	F[11] = 0.0;
	F[14] = 0.0;
	F[15] = 1.0;

	/*  state prediction */
	x_pred[0] = self->x[0] + a21;
	x_pred[1] = self->x[1] + s;
	x_pred[2] = self->x[2];
	x_pred[3] = self->x[3];

	/*  covariance prediction */
	for (i = 0; i < 4; i++) {
		a21 = F[i + 4];
		s = F[i + 8];
		a22 = F[i + 12];
		for (i1 = 0; i1 < 4; i1++) {
			jA = i1 << 2;
			b_F[i + jA] = ((F[i] * self->P[jA] + a21 * self->P[jA + 1]) + s * self->P[jA + 2])
					+ a22 * self->P[jA + 3];
		}
	}

	for (i = 0; i < 4; i++) {
		a21 = b_F[i + 4];
		s = b_F[i + 8];
		a22 = b_F[i + 12];
		for (i1 = 0; i1 < 4; i1++) {
			jA = i + (i1 << 2);
			self->P[jA] = (((b_F[i] * F[i1] + a21 * F[i1 + 4]) + s * F[i1 + 8])
					+ a22 * F[i1 + 12]) + dv[jA];
		}
	}

	if (measurements[0] != 999999999) {
		/*  measurement residual */
		/*  measurement residual covariance */
		/* Kalman gain */
		for (i = 0; i < 4; i++) {
			i2 = b_a[i + 4];
			i3 = b_a[i + 8];
			i4 = b_a[i + 12];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 2;
				b_F[i + jA] = (((float) b_a[i] * self->P[jA]
						+ (float) i2 * self->P[jA + 1]) + (float) i3 * self->P[jA + 2])
						+ (float) i4 * self->P[jA + 3];
			}

			a21 = b_F[i + 4];
			s = b_F[i + 8];
			a22 = b_F[i + 12];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 2;
				r2 = i + jA;
				i2 = iv1[jA + 1];
				i3 = iv1[jA + 2];
				i4 = iv1[jA + 3];
				F[r2] = (((b_F[i] * (float) iv1[jA] + a21 * (float) i2)
						+ s * (float) i3) + a22 * (float) i4) + dv2[r2];
				K[i + jA] = ((self->P[i] * (float) iv1[jA] + self->P[i + 4] * (float) i2)
						+ self->P[i + 8] * (float) i3) + self->P[i + 12] * (float) i4;
			}

			ipiv[i] = (signed char) (i + 1);
		}

		for (j = 0; j < 3; j++) {
			mmj_tmp = 2 - j;
			r1 = j * 5;
			jj = j * 5;
			jp1j = r1 + 2;
			r2 = 4 - j;
			jA = 0;
			ix = r1;
			a21 = fabs(F[jj]);
			for (K_tmp = 2; K_tmp <= r2; K_tmp++) {
				ix++;
				s = fabs(F[ix]);
				if (s > a21) {
					jA = K_tmp - 1;
					a21 = s;
				}
			}

			if (F[jj + jA] != 0.0) {
				if (jA != 0) {
					r2 = j + jA;
					ipiv[j] = (signed char) (r2 + 1);
					a21 = F[j];
					F[j] = F[r2];
					F[r2] = a21;
					ix = j + 4;
					r2 += 4;
					a21 = F[ix];
					F[ix] = F[r2];
					F[r2] = a21;
					ix += 4;
					r2 += 4;
					a21 = F[ix];
					F[ix] = F[r2];
					F[r2] = a21;
					ix += 4;
					r2 += 4;
					a21 = F[ix];
					F[ix] = F[r2];
					F[r2] = a21;
				}

				i = (jj - j) + 4;
				for (r2 = jp1j; r2 <= i; r2++) {
					F[r2 - 1] /= F[jj];
				}
			}

			r2 = r1 + 4;
			jA = jj;
			for (r1 = 0; r1 <= mmj_tmp; r1++) {
				a21 = F[r2];
				if (F[r2] != 0.0) {
					ix = jj + 1;
					i = jA + 6;
					i1 = (jA - j) + 8;
					for (jp1j = i; jp1j <= i1; jp1j++) {
						F[jp1j - 1] += F[ix] * -a21;
						ix++;
					}
				}

				r2 += 4;
				jA += 4;
			}
		}

		for (j = 0; j < 4; j++) {
			jA = j << 2;
			r2 = jA - 1;
			for (K_tmp = 0; K_tmp < j; K_tmp++) {
				r1 = K_tmp << 2;
				i = K_tmp + jA;
				if (F[i] != 0.0) {
					K[r2 + 1] -= F[i] * K[r1];
					K[r2 + 2] -= F[i] * K[r1 + 1];
					K[r2 + 3] -= F[i] * K[r1 + 2];
					K[r2 + 4] -= F[i] * K[r1 + 3];
				}
			}

			a21 = 1.0 / F[j + jA];
			K[r2 + 1] *= a21;
			K[r2 + 2] *= a21;
			K[r2 + 3] *= a21;
			K[r2 + 4] *= a21;
		}

		for (j = 3; j >= 0; j--) {
			jA = (j << 2) - 1;
			i = j + 2;
			for (K_tmp = i; K_tmp < 5; K_tmp++) {
				r1 = (K_tmp - 1) << 2;
				i1 = K_tmp + jA;
				if (F[i1] != 0.0) {
					K[jA + 1] -= F[i1] * K[r1];
					K[jA + 2] -= F[i1] * K[r1 + 1];
					K[jA + 3] -= F[i1] * K[r1 + 2];
					K[jA + 4] -= F[i1] * K[r1 + 3];
				}
			}
		}

		for (j = 2; j >= 0; j--) {
			if (ipiv[j] != j + 1) {
				r2 = j << 2;
				a21 = K[r2];
				K_tmp = (ipiv[j] - 1) << 2;
				K[r2] = K[K_tmp];
				K[K_tmp] = a21;
				jA = r2 + 1;
				a21 = K[jA];
				jp1j = K_tmp + 1;
				K[jA] = K[jp1j];
				K[jp1j] = a21;
				jA = r2 + 2;
				a21 = K[jA];
				jp1j = K_tmp + 2;
				K[jA] = K[jp1j];
				K[jp1j] = a21;
				r2 += 3;
				a21 = K[r2];
				K_tmp += 3;
				K[r2] = K[K_tmp];
				K[K_tmp] = a21;
			}
		}

		/*  Update state */
		for (i = 0; i < 4; i++) {
			S[i] = measurements[i]
					- ((((float) b_a[i] * x_pred[0]
							+ (float) b_a[i + 4] * x_pred[1])
							+ (float) b_a[i + 8] * x_pred[2])
							+ (float) b_a[i + 12] * x_pred[3]);
		}

		for (i = 0; i < 4; i++) {
			self->x[i] = x_pred[i]
					+ (((K[i] * S[0] + K[i + 4] * S[1]) + K[i + 8] * S[2])
							+ K[i + 12] * S[3]);
		}

		/*  Update covariance */
		memset(&F[0], 0, 16U * sizeof(float));
		F[0] = 1.0;
		F[5] = 1.0;
		F[10] = 1.0;
		F[15] = 1.0;
		for (i = 0; i < 4; i++) {
			a21 = K[i + 4];
			s = K[i + 8];
			a22 = K[i + 12];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 2;
				r2 = i + jA;
				b_F[r2] = F[r2]
						- (((K[i] * (float) b_a[jA]
								+ a21 * (float) b_a[jA + 1])
								+ s * (float) b_a[jA + 2])
								+ a22 * (float) b_a[jA + 3]);
			}

			a21 = b_F[i + 4];
			s = b_F[i + 8];
			a22 = b_F[i + 12];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 2;
				F[i + jA] = ((b_F[i] * self->P[jA] + a21 * self->P[jA + 1]) + s * self->P[jA + 2])
						+ a22 * self->P[jA + 3];
			}
		}

		memcpy(&self->P[0], &F[0], 16U * sizeof(float));
	} else {
		/*  measurement residual */
		/*  measurement residual covariance */
		for (i = 0; i < 2; i++) {
			i2 = a[i + 4];
			i3 = a[i + 6];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 2;
				c_a[i + (i1 << 1)] = ((0.0 * self->P[jA] + 0.0 * self->P[jA + 1])
						+ (float) i2 * self->P[jA + 2]) + (float) i3 * self->P[jA + 3];
			}

			a21 = c_a[i + 2];
			s = c_a[i + 4];
			a22 = c_a[i + 6];
			for (i1 = 0; i1 < 2; i1++) {
				jA = i1 << 2;
				r2 = i + (i1 << 1);
				S[r2] = (((c_a[i] * (float) iv[jA] + a21 * (float) iv[jA + 1])
						+ s * (float) iv[jA + 2]) + a22 * (float) iv[jA + 3])
						+ dv1[r2];
			}
		}

		/* Kalman gain */
		for (i = 0; i < 4; i++) {
			for (i1 = 0; i1 < 2; i1++) {
				jA = i1 << 2;
				c_a[i + jA] = ((self->P[i] * (float) iv[jA]
						+ self->P[i + 4] * (float) iv[jA + 1])
						+ self->P[i + 8] * (float) iv[jA + 2])
						+ self->P[i + 12] * (float) iv[jA + 3];
			}
		}

		if (fabs(S[1]) > fabs(S[0])) {
			r1 = 1;
			r2 = 0;
		} else {
			r1 = 0;
			r2 = 1;
		}

		a21 = S[r2] / S[r1];
		s = S[r1 + 2];
		a22 = S[r2 + 2] - a21 * s;
		K_tmp = r1 << 2;
		b_K[K_tmp] = c_a[0] / S[r1];
		jp1j = r2 << 2;
		b_K[jp1j] = (c_a[4] - b_K[K_tmp] * s) / a22;
		b_K[K_tmp] -= b_K[jp1j] * a21;
		r2 = K_tmp + 1;
		b_K[r2] = c_a[1] / S[r1];
		jA = jp1j + 1;
		b_K[jA] = (c_a[5] - b_K[r2] * s) / a22;
		b_K[r2] -= b_K[jA] * a21;
		r2 = K_tmp + 2;
		b_K[r2] = c_a[2] / S[r1];
		jA = jp1j + 2;
		b_K[jA] = (c_a[6] - b_K[r2] * s) / a22;
		b_K[r2] -= b_K[jA] * a21;
		K_tmp += 3;
		b_K[K_tmp] = c_a[3] / S[r1];
		jp1j += 3;
		b_K[jp1j] = (c_a[7] - b_K[K_tmp] * s) / a22;
		b_K[K_tmp] -= b_K[jp1j] * a21;

		/*  Update state */
		for (i = 0; i < 2; i++) {
			b_measurements[i] = measurements[i + 2]
					- (((0.0 * x_pred[0] + 0.0 * x_pred[1])
							+ (float) a[i + 4] * x_pred[2])
							+ (float) a[i + 6] * x_pred[3]);
		}

		for (i = 0; i < 4; i++) {
			self->x[i] = x_pred[i]
					+ (b_K[i] * b_measurements[0]
							+ b_K[i + 4] * b_measurements[1]);
		}

		/*  Update covariance */
		memset(&F[0], 0, 16U * sizeof(float));
		F[0] = 1.0;
		F[5] = 1.0;
		F[10] = 1.0;
		F[15] = 1.0;
		for (i = 0; i < 4; i++) {
			a21 = b_K[i + 4];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 1;
				r2 = i + (i1 << 2);
				b_F[r2] = F[r2]
						- (b_K[i] * (float) a[jA] + a21 * (float) a[jA + 1]);
			}

			a21 = b_F[i + 4];
			s = b_F[i + 8];
			a22 = b_F[i + 12];
			for (i1 = 0; i1 < 4; i1++) {
				jA = i1 << 2;
				F[i + jA] = ((b_F[i] * self->P[jA] + a21 * self->P[jA + 1]) + s * self->P[jA + 2])
						+ a22 * self->P[jA + 3];
			}
		}

		memcpy(&self->P[0], &F[0], 16U * sizeof(float));
	}
}

void getPositionEstimate(PositionEstimator *self, float *xEst, float *yEst) {
	*xEst = self->x[0];
	*yEst = self->x[1];
}
