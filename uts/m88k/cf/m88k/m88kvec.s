; Motorola M88000 vector table	text
	global	_M88Kvec
_M88Kvec:
	global	M88Kvec
M88Kvec:
	stcr	r1,cr18		; 0 Reset vector (unused)
	br	_start

	br	interrupt	; 1 interrupt vector
	br	interrupt

	br	Xcaccflt	; 2 Code fault vector
	br	Xcaccflt

	br	Xdaccflt	; 3 Data fault vector
	br	Xdaccflt

	br	Xaddr		; 4 Misaligned access error
	br	Xaddr

	br	Xinst		; 5 Unimplemented opcode
	br	Xinst

	br	Xpriv		; 6 Privelege violation vector
	br	Xpriv

	br	Xtbnd		; 7 Trap on bounds violation vector
	br	Xtbnd

	br	Xdivz		; 8 Integer divide by zero vector
	br	Xdivz

	br	Xiovf		; 9 Integer overflow vector
	br	Xiovf

	stcr	r1,cr18		; 10 Error Exception
	bsr	Xtrap

	stcr	r1,cr18		; 11
	bsr	Xtrap

	stcr	r1,cr18		; 12
	bsr	Xtrap

	stcr	r1,cr18		; 13
	bsr	Xtrap

	stcr	r1,cr18		; 14
	bsr	Xtrap

	stcr	r1,cr18		; 15
	bsr	Xtrap

	stcr	r1,cr18		; 16
	bsr	Xtrap

	stcr	r1,cr18		; 17
	bsr	Xtrap

	stcr	r1,cr18		; 18
	bsr	Xtrap

	stcr	r1,cr18		; 19
	bsr	Xtrap

	stcr	r1,cr18		; 20
	bsr	Xtrap

	stcr	r1,cr18		; 21
	bsr	Xtrap

	stcr	r1,cr18		; 22
	bsr	Xtrap

	stcr	r1,cr18		; 23
	bsr	Xtrap

	stcr	r1,cr18		; 24
	bsr	Xtrap

	stcr	r1,cr18		; 25
	bsr	Xtrap

	stcr	r1,cr18		; 26
	bsr	Xtrap

	stcr	r1,cr18		; 27
	bsr	Xtrap

	stcr	r1,cr18		; 28
	bsr	Xtrap

	stcr	r1,cr18		; 29
	bsr	Xtrap

	stcr	r1,cr18		; 30
	bsr	Xtrap

	stcr	r1,cr18		; 31
	bsr	Xtrap

	stcr	r1,cr18		; 32
	bsr	Xtrap

	stcr	r1,cr18		; 33
	bsr	Xtrap

	stcr	r1,cr18		; 34
	bsr	Xtrap

	stcr	r1,cr18		; 35
	bsr	Xtrap

	stcr	r1,cr18		; 36
	bsr	Xtrap

	stcr	r1,cr18		; 37
	bsr	Xtrap

	stcr	r1,cr18		; 38
	bsr	Xtrap

	stcr	r1,cr18		; 39
	bsr	Xtrap

	stcr	r1,cr18		; 40
	bsr	Xtrap

	stcr	r1,cr18		; 41
	bsr	Xtrap

	stcr	r1,cr18		; 42
	bsr	Xtrap

	stcr	r1,cr18		; 43
	bsr	Xtrap

	stcr	r1,cr18		; 44
	bsr	Xtrap

	stcr	r1,cr18		; 45
	bsr	Xtrap

	stcr	r1,cr18		; 46
	bsr	Xtrap

	stcr	r1,cr18		; 47
	bsr	Xtrap

	stcr	r1,cr18		; 48
	bsr	Xtrap

	stcr	r1,cr18		; 49
	bsr	Xtrap

	stcr	r1,cr18		; 50
	bsr	Xtrap

	stcr	r1,cr18		; 51
	bsr	Xtrap

	stcr	r1,cr18		; 52
	bsr	Xtrap

	stcr	r1,cr18		; 53
	bsr	Xtrap

	stcr	r1,cr18		; 54
	bsr	Xtrap

	stcr	r1,cr18		; 55
	bsr	Xtrap

	stcr	r1,cr18		; 56
	bsr	Xtrap

	stcr	r1,cr18		; 57
	bsr	Xtrap

	stcr	r1,cr18		; 58
	bsr	Xtrap

	stcr	r1,cr18		; 59
	bsr	Xtrap

	stcr	r1,cr18		; 60
	bsr	Xtrap

	stcr	r1,cr18		; 61
	bsr	Xtrap

	stcr	r1,cr18		; 62
	bsr	Xtrap

	stcr	r1,cr18		; 63
	bsr	Xtrap

	stcr	r1,cr18		; 64
	bsr	Xtrap

	stcr	r1,cr18		; 65
	bsr	Xtrap

	stcr	r1,cr18		; 66
	bsr	Xtrap

	stcr	r1,cr18		; 67
	bsr	Xtrap

	stcr	r1,cr18		; 68
	bsr	Xtrap

	stcr	r1,cr18		; 69
	bsr	Xtrap

	stcr	r1,cr18		; 70
	bsr	Xtrap

	stcr	r1,cr18		; 71
	bsr	Xtrap

	stcr	r1,cr18		; 72
	bsr	Xtrap

	stcr	r1,cr18		; 73
	bsr	Xtrap

	stcr	r1,cr18		; 74
	bsr	Xtrap

	stcr	r1,cr18		; 75
	bsr	Xtrap

	stcr	r1,cr18		; 76
	bsr	Xtrap

	stcr	r1,cr18		; 77
	bsr	Xtrap

	stcr	r1,cr18		; 78
	bsr	Xtrap

	stcr	r1,cr18		; 79
	bsr	Xtrap

	stcr	r1,cr18		; 80
	bsr	Xtrap

	stcr	r1,cr18		; 81
	bsr	Xtrap

	stcr	r1,cr18		; 82
	bsr	Xtrap

	stcr	r1,cr18		; 83
	bsr	Xtrap

	stcr	r1,cr18		; 84
	bsr	Xtrap

	stcr	r1,cr18		; 85
	bsr	Xtrap

	stcr	r1,cr18		; 86
	bsr	Xtrap

	stcr	r1,cr18		; 87
	bsr	Xtrap

	stcr	r1,cr18		; 88
	bsr	Xtrap

	stcr	r1,cr18		; 89
	bsr	Xtrap

	stcr	r1,cr18		; 90
	bsr	Xtrap

	stcr	r1,cr18		; 91
	bsr	Xtrap

	stcr	r1,cr18		; 92
	bsr	Xtrap

	stcr	r1,cr18		; 93
	bsr	Xtrap

	stcr	r1,cr18		; 94
	bsr	Xtrap

	stcr	r1,cr18		; 95
	bsr	Xtrap

	stcr	r1,cr18		; 96
	bsr	Xtrap

	stcr	r1,cr18		; 97
	bsr	Xtrap

	stcr	r1,cr18		; 98
	bsr	Xtrap

	stcr	r1,cr18		; 99
	bsr	Xtrap

	stcr	r1,cr18		; 100
	bsr	Xtrap

	stcr	r1,cr18		; 101
	bsr	Xtrap

	stcr	r1,cr18		; 102
	bsr	Xtrap

	stcr	r1,cr18		; 103
	bsr	Xtrap

	stcr	r1,cr18		; 104
	bsr	Xtrap

	stcr	r1,cr18		; 105
	bsr	Xtrap

	stcr	r1,cr18		; 106
	bsr	Xtrap

	stcr	r1,cr18		; 107
	bsr	Xtrap

	stcr	r1,cr18		; 108
	bsr	Xtrap

	stcr	r1,cr18		; 109
	bsr	Xtrap

	stcr	r1,cr18		; 110
	bsr	Xtrap

	stcr	r1,cr18		; 111
	bsr	Xtrap

	stcr	r1,cr18		; 112
	bsr	Xtrap

	stcr	r1,cr18		; 113
	bsr	Xtrap

	br.n	_Xsfu1pr	; 114 SFU #1 Precise exception
	stcr	r1,cr18		; save r1

	br.n	_Xsfu1imp	; 115 SFU #1 Imprecise exception
	stcr	r1,cr18		; save r1

	stcr	r1,cr18		; 116
	bsr	Xtrap

	stcr	r1,cr18		; 117
	bsr	Xtrap

	stcr	r1,cr18		; 118
	bsr	Xtrap

	stcr	r1,cr18		; 119
	bsr	Xtrap

	stcr	r1,cr18		; 120
	bsr	Xtrap

	stcr	r1,cr18		; 121
	bsr	Xtrap

	stcr	r1,cr18		; 122
	bsr	Xtrap

	stcr	r1,cr18		; 123
	bsr	Xtrap

	stcr	r1,cr18		; 124
	bsr	Xtrap

	stcr	r1,cr18		; 125
	bsr	Xtrap

	stcr	r1,cr18		; 126
	bsr	Xtrap

	stcr	r1,cr18		; 127
	bsr	Xtrap

	stcr	r1,cr18		; 128
	bsr	Xtrap

	stcr	r1,cr18		; 129
	bsr	Xtrap

	stcr	r1,cr18		; 130
	bsr	Xtrap

	stcr	r1,cr18		; 131
	bsr	Xtrap

	stcr	r1,cr18		; 132
	bsr	Xtrap

	stcr	r1,cr18		; 133
	bsr	Xtrap

	stcr	r1,cr18		; 134
	bsr	Xtrap

	stcr	r1,cr18		; 135
	bsr	Xtrap

	stcr	r1,cr18		; 136
	bsr	Xtrap

	stcr	r1,cr18		; 137
	bsr	Xtrap

	stcr	r1,cr18		; 138
	bsr	Xtrap

	stcr	r1,cr18		; 139
	bsr	Xtrap

	stcr	r1,cr18		; 140
	bsr	Xtrap

	stcr	r1,cr18		; 141
	bsr	Xtrap

	stcr	r1,cr18		; 142
	bsr	Xtrap

	stcr	r1,cr18		; 143
	bsr	Xtrap

	stcr	r1,cr18		; 144
	bsr	Xtrap

	stcr	r1,cr18		; 145
	bsr	Xtrap

	stcr	r1,cr18		; 146
	bsr	Xtrap

	stcr	r1,cr18		; 147
	bsr	Xtrap

	stcr	r1,cr18		; 148
	bsr	Xtrap

	stcr	r1,cr18		; 149
	bsr	Xtrap

	stcr	r1,cr18		; 150
	bsr	Xtrap

	stcr	r1,cr18		; 151
	bsr	Xtrap

	stcr	r1,cr18		; 152
	bsr	Xtrap

	stcr	r1,cr18		; 153
	bsr	Xtrap

	stcr	r1,cr18		; 154
	bsr	Xtrap

	stcr	r1,cr18		; 155
	bsr	Xtrap

	stcr	r1,cr18		; 156
	bsr	Xtrap

	stcr	r1,cr18		; 157
	bsr	Xtrap

	stcr	r1,cr18		; 158
	bsr	Xtrap

	stcr	r1,cr18		; 159
	bsr	Xtrap

	stcr	r1,cr18		; 160
	bsr	Xtrap

	stcr	r1,cr18		; 161
	bsr	Xtrap

	stcr	r1,cr18		; 162
	bsr	Xtrap

	stcr	r1,cr18		; 163
	bsr	Xtrap

	stcr	r1,cr18		; 164
	bsr	Xtrap

	stcr	r1,cr18		; 165
	bsr	Xtrap

	stcr	r1,cr18		; 166
	bsr	Xtrap

	stcr	r1,cr18		; 167
	bsr	Xtrap

	stcr	r1,cr18		; 168
	bsr	Xtrap

	stcr	r1,cr18		; 169
	bsr	Xtrap

	stcr	r1,cr18		; 170
	bsr	Xtrap

	stcr	r1,cr18		; 171
	bsr	Xtrap

	stcr	r1,cr18		; 172
	bsr	Xtrap

	stcr	r1,cr18		; 173
	bsr	Xtrap

	stcr	r1,cr18		; 174
	bsr	Xtrap

	stcr	r1,cr18		; 175
	bsr	Xtrap

	stcr	r1,cr18		; 176
	bsr	Xtrap

	stcr	r1,cr18		; 177
	bsr	Xtrap

	stcr	r1,cr18		; 178
	bsr	Xtrap

	stcr	r1,cr18		; 179
	bsr	Xtrap

	stcr	r1,cr18		; 180
	bsr	Xtrap

	stcr	r1,cr18		; 181
	bsr	Xtrap

	stcr	r1,cr18		; 182
	bsr	Xtrap

	stcr	r1,cr18		; 183
	bsr	Xtrap

	stcr	r1,cr18		; 184
	bsr	Xtrap

	stcr	r1,cr18		; 185
	bsr	Xtrap

	stcr	r1,cr18		; 186
	bsr	Xtrap

	stcr	r1,cr18		; 187
	bsr	Xtrap

	stcr	r1,cr18		; 188
	bsr	Xtrap

	stcr	r1,cr18		; 189
	bsr	Xtrap

	stcr	r1,cr18		; 190
	bsr	Xtrap

	stcr	r1,cr18		; 191
	bsr	Xtrap

	stcr	r1,cr18		; 192
	bsr	Xtrap

	stcr	r1,cr18		; 193
	bsr	Xtrap

	stcr	r1,cr18		; 194
	bsr	Xtrap

	stcr	r1,cr18		; 195
	bsr	Xtrap

	stcr	r1,cr18		; 196
	bsr	Xtrap

	stcr	r1,cr18		; 197
	bsr	Xtrap

	stcr	r1,cr18		; 198
	bsr	Xtrap

	stcr	r1,cr18		; 199
	bsr	Xtrap

	stcr	r1,cr18		; 200
	bsr	Xtrap

	stcr	r1,cr18		; 201
	bsr	Xtrap

	stcr	r1,cr18		; 202
	bsr	Xtrap

	stcr	r1,cr18		; 203
	bsr	Xtrap

	stcr	r1,cr18		; 204
	bsr	Xtrap

	stcr	r1,cr18		; 205
	bsr	Xtrap

	stcr	r1,cr18		; 206
	bsr	Xtrap

	stcr	r1,cr18		; 207
	bsr	Xtrap

	stcr	r1,cr18		; 208
	bsr	Xtrap

	stcr	r1,cr18		; 209
	bsr	Xtrap

	stcr	r1,cr18		; 210
	bsr	Xtrap

	stcr	r1,cr18		; 211
	bsr	Xtrap

	stcr	r1,cr18		; 212
	bsr	Xtrap

	stcr	r1,cr18		; 213
	bsr	Xtrap

	stcr	r1,cr18		; 214
	bsr	Xtrap

	stcr	r1,cr18		; 215
	bsr	Xtrap

	stcr	r1,cr18		; 216
	bsr	Xtrap

	stcr	r1,cr18		; 217
	bsr	Xtrap

	stcr	r1,cr18		; 218
	bsr	Xtrap

	stcr	r1,cr18		; 219
	bsr	Xtrap

	stcr	r1,cr18		; 220
	bsr	Xtrap

	stcr	r1,cr18		; 221
	bsr	Xtrap

	stcr	r1,cr18		; 222
	bsr	Xtrap

	stcr	r1,cr18		; 223
	bsr	Xtrap

	stcr	r1,cr18		; 224
	bsr	Xtrap

	stcr	r1,cr18		; 225
	bsr	Xtrap

	stcr	r1,cr18		; 226
	bsr	Xtrap

	stcr	r1,cr18		; 227
	bsr	Xtrap

	stcr	r1,cr18		; 228
	bsr	Xtrap

	stcr	r1,cr18		; 229
	bsr	Xtrap

	stcr	r1,cr18		; 230
	bsr	Xtrap

	stcr	r1,cr18		; 231
	bsr	Xtrap

	stcr	r1,cr18		; 232
	bsr	Xtrap

	stcr	r1,cr18		; 233
	bsr	Xtrap

	stcr	r1,cr18		; 234
	bsr	Xtrap

	stcr	r1,cr18		; 235
	bsr	Xtrap

	stcr	r1,cr18		; 236
	bsr	Xtrap

	stcr	r1,cr18		; 237
	bsr	Xtrap

	stcr	r1,cr18		; 238
	bsr	Xtrap

	stcr	r1,cr18		; 239
	bsr	Xtrap

	stcr	r1,cr18		; 240
	bsr	Xtrap

	stcr	r1,cr18		; 241
	bsr	Xtrap

	stcr	r1,cr18		; 242
	bsr	Xtrap

	stcr	r1,cr18		; 243
	bsr	Xtrap

	stcr	r1,cr18		; 244
	bsr	Xtrap

	stcr	r1,cr18		; 245
	bsr	Xtrap

	stcr	r1,cr18		; 246
	bsr	Xtrap

	stcr	r1,cr18		; 247
	bsr	Xtrap

	stcr	r1,cr18		; 248
	bsr	Xtrap

	stcr	r1,cr18		; 249
	bsr	Xtrap

	stcr	r1,cr18		; 250
	bsr	Xtrap

	stcr	r1,cr18		; 251
	bsr	Xtrap

	stcr	r1,cr18		; 252
	bsr	Xtrap

	stcr	r1,cr18		; 253
	bsr	Xtrap

	stcr	r1,cr18		; 254
	bsr	Xtrap

	stcr	r1,cr18		; 255
	bsr	Xtrap

	stcr	r1,cr18		; 256
	bsr	Xtrap

	stcr	r1,cr18		; 257
	bsr	Xtrap

	stcr	r1,cr18		; 258
	bsr	Xtrap

	stcr	r1,cr18		; 259
	bsr	Xtrap

	stcr	r1,cr18		; 260
	bsr	Xtrap

	stcr	r1,cr18		; 261
	bsr	Xtrap

	stcr	r1,cr18		; 262
	bsr	Xtrap

	stcr	r1,cr18		; 263
	bsr	Xtrap

	stcr	r1,cr18		; 264
	bsr	Xtrap

	stcr	r1,cr18		; 265
	bsr	Xtrap

	stcr	r1,cr18		; 266
	bsr	Xtrap

	stcr	r1,cr18		; 267
	bsr	Xtrap

	stcr	r1,cr18		; 268
	bsr	Xtrap

	stcr	r1,cr18		; 269
	bsr	Xtrap

	stcr	r1,cr18		; 270
	bsr	Xtrap

	stcr	r1,cr18		; 271
	bsr	Xtrap

	stcr	r1,cr18		; 272
	bsr	Xtrap

	stcr	r1,cr18		; 273
	bsr	Xtrap

	stcr	r1,cr18		; 274
	bsr	Xtrap

	stcr	r1,cr18		; 275
	bsr	Xtrap

	stcr	r1,cr18		; 276
	bsr	Xtrap

	stcr	r1,cr18		; 277
	bsr	Xtrap

	stcr	r1,cr18		; 278
	bsr	Xtrap

	stcr	r1,cr18		; 279
	bsr	Xtrap

	stcr	r1,cr18		; 280
	bsr	Xtrap

	stcr	r1,cr18		; 281
	bsr	Xtrap

	stcr	r1,cr18		; 282
	bsr	Xtrap

	stcr	r1,cr18		; 283
	bsr	Xtrap

	stcr	r1,cr18		; 284
	bsr	Xtrap

	stcr	r1,cr18		; 285
	bsr	Xtrap

	stcr	r1,cr18		; 286
	bsr	Xtrap

	stcr	r1,cr18		; 287
	bsr	Xtrap

	stcr	r1,cr18		; 288
	bsr	Xtrap

	stcr	r1,cr18		; 289
	bsr	Xtrap

	stcr	r1,cr18		; 290
	bsr	Xtrap

	stcr	r1,cr18		; 291
	bsr	Xtrap

	stcr	r1,cr18		; 292
	bsr	Xtrap

	stcr	r1,cr18		; 293
	bsr	Xtrap

	stcr	r1,cr18		; 294
	bsr	Xtrap

	stcr	r1,cr18		; 295
	bsr	Xtrap

	stcr	r1,cr18		; 296
	bsr	Xtrap

	stcr	r1,cr18		; 297
	bsr	Xtrap

	stcr	r1,cr18		; 298
	bsr	Xtrap

	stcr	r1,cr18		; 299
	bsr	Xtrap

	stcr	r1,cr18		; 300
	bsr	Xtrap

	stcr	r1,cr18		; 301
	bsr	Xtrap

	stcr	r1,cr18		; 302
	bsr	Xtrap

	stcr	r1,cr18		; 303
	bsr	Xtrap

	stcr	r1,cr18		; 304
	bsr	Xtrap

	stcr	r1,cr18		; 305
	bsr	Xtrap

	stcr	r1,cr18		; 306
	bsr	Xtrap

	stcr	r1,cr18		; 307
	bsr	Xtrap

	stcr	r1,cr18		; 308
	bsr	Xtrap

	stcr	r1,cr18		; 309
	bsr	Xtrap

	stcr	r1,cr18		; 310
	bsr	Xtrap

	stcr	r1,cr18		; 311
	bsr	Xtrap

	stcr	r1,cr18		; 312
	bsr	Xtrap

	stcr	r1,cr18		; 313
	bsr	Xtrap

	stcr	r1,cr18		; 314
	bsr	Xtrap

	stcr	r1,cr18		; 315
	bsr	Xtrap

	stcr	r1,cr18		; 316
	bsr	Xtrap

	stcr	r1,cr18		; 317
	bsr	Xtrap

	stcr	r1,cr18		; 318
	bsr	Xtrap

	stcr	r1,cr18		; 319
	bsr	Xtrap

	stcr	r1,cr18		; 320
	bsr	Xtrap

	stcr	r1,cr18		; 321
	bsr	Xtrap

	stcr	r1,cr18		; 322
	bsr	Xtrap

	stcr	r1,cr18		; 323
	bsr	Xtrap

	stcr	r1,cr18		; 324
	bsr	Xtrap

	stcr	r1,cr18		; 325
	bsr	Xtrap

	stcr	r1,cr18		; 326
	bsr	Xtrap

	stcr	r1,cr18		; 327
	bsr	Xtrap

	stcr	r1,cr18		; 328
	bsr	Xtrap

	stcr	r1,cr18		; 329
	bsr	Xtrap

	stcr	r1,cr18		; 330
	bsr	Xtrap

	stcr	r1,cr18		; 331
	bsr	Xtrap

	stcr	r1,cr18		; 332
	bsr	Xtrap

	stcr	r1,cr18		; 333
	bsr	Xtrap

	stcr	r1,cr18		; 334
	bsr	Xtrap

	stcr	r1,cr18		; 335
	bsr	Xtrap

	stcr	r1,cr18		; 336
	bsr	Xtrap

	stcr	r1,cr18		; 337
	bsr	Xtrap

	stcr	r1,cr18		; 338
	bsr	Xtrap

	stcr	r1,cr18		; 339
	bsr	Xtrap

	stcr	r1,cr18		; 340
	bsr	Xtrap

	stcr	r1,cr18		; 341
	bsr	Xtrap

	stcr	r1,cr18		; 342
	bsr	Xtrap

	stcr	r1,cr18		; 343
	bsr	Xtrap

	stcr	r1,cr18		; 344
	bsr	Xtrap

	stcr	r1,cr18		; 345
	bsr	Xtrap

	stcr	r1,cr18		; 346
	bsr	Xtrap

	stcr	r1,cr18		; 347
	bsr	Xtrap

	stcr	r1,cr18		; 348
	bsr	Xtrap

	stcr	r1,cr18		; 349
	bsr	Xtrap

	stcr	r1,cr18		; 350
	bsr	Xtrap

	stcr	r1,cr18		; 351
	bsr	Xtrap

	stcr	r1,cr18		; 352
	bsr	Xtrap

	stcr	r1,cr18		; 353
	bsr	Xtrap

	stcr	r1,cr18		; 354
	bsr	Xtrap

	stcr	r1,cr18		; 355
	bsr	Xtrap

	stcr	r1,cr18		; 356
	bsr	Xtrap

	stcr	r1,cr18		; 357
	bsr	Xtrap

	stcr	r1,cr18		; 358
	bsr	Xtrap

	stcr	r1,cr18		; 359
	bsr	Xtrap

	stcr	r1,cr18		; 360
	bsr	Xtrap

	stcr	r1,cr18		; 361
	bsr	Xtrap

	stcr	r1,cr18		; 362
	bsr	Xtrap

	stcr	r1,cr18		; 363
	bsr	Xtrap

	stcr	r1,cr18		; 364
	bsr	Xtrap

	stcr	r1,cr18		; 365
	bsr	Xtrap

	stcr	r1,cr18		; 366
	bsr	Xtrap

	stcr	r1,cr18		; 367
	bsr	Xtrap

	stcr	r1,cr18		; 368
	bsr	Xtrap

	stcr	r1,cr18		; 369
	bsr	Xtrap

	stcr	r1,cr18		; 370
	bsr	Xtrap

	stcr	r1,cr18		; 371
	bsr	Xtrap

	stcr	r1,cr18		; 372
	bsr	Xtrap

	stcr	r1,cr18		; 373
	bsr	Xtrap

	stcr	r1,cr18		; 374
	bsr	Xtrap

	stcr	r1,cr18		; 375
	bsr	Xtrap

	stcr	r1,cr18		; 376
	bsr	Xtrap

	stcr	r1,cr18		; 377
	bsr	Xtrap

	stcr	r1,cr18		; 378
	bsr	Xtrap

	stcr	r1,cr18		; 379
	bsr	Xtrap

	stcr	r1,cr18		; 380
	bsr	Xtrap

	stcr	r1,cr18		; 381
	bsr	Xtrap

	stcr	r1,cr18		; 382
	bsr	Xtrap

	stcr	r1,cr18		; 383
	bsr	Xtrap

	stcr	r1,cr18		; 384
	bsr	Xtrap

	stcr	r1,cr18		; 385
	bsr	Xtrap

	stcr	r1,cr18		; 386
	bsr	Xtrap

	stcr	r1,cr18		; 387
	bsr	Xtrap

	stcr	r1,cr18		; 388
	bsr	Xtrap

	stcr	r1,cr18		; 389
	bsr	Xtrap

	stcr	r1,cr18		; 390
	bsr	Xtrap

	stcr	r1,cr18		; 391
	bsr	Xtrap

	stcr	r1,cr18		; 392
	bsr	Xtrap

	stcr	r1,cr18		; 393
	bsr	Xtrap

	stcr	r1,cr18		; 394
	bsr	Xtrap

	stcr	r1,cr18		; 395
	bsr	Xtrap

	stcr	r1,cr18		; 396
	bsr	Xtrap

	stcr	r1,cr18		; 397
	bsr	Xtrap

	stcr	r1,cr18		; 398
	bsr	Xtrap

	stcr	r1,cr18		; 399
	bsr	Xtrap

	stcr	r1,cr18		; 400
	bsr	Xtrap

	stcr	r1,cr18		; 401
	bsr	Xtrap

	stcr	r1,cr18		; 402
	bsr	Xtrap

	stcr	r1,cr18		; 403
	bsr	Xtrap

	stcr	r1,cr18		; 404
	bsr	Xtrap

	stcr	r1,cr18		; 405
	bsr	Xtrap

	stcr	r1,cr18		; 406
	bsr	Xtrap

	stcr	r1,cr18		; 407
	bsr	Xtrap

	stcr	r1,cr18		; 408
	bsr	Xtrap

	stcr	r1,cr18		; 409
	bsr	Xtrap

	stcr	r1,cr18		; 410
	bsr	Xtrap

	stcr	r1,cr18		; 411
	bsr	Xtrap

	stcr	r1,cr18		; 412
	bsr	Xtrap

	stcr	r1,cr18		; 413
	bsr	Xtrap

	stcr	r1,cr18		; 414
	bsr	Xtrap

	stcr	r1,cr18		; 415
	bsr	Xtrap

	stcr	r1,cr18		; 416
	bsr	Xtrap

	stcr	r1,cr18		; 417
	bsr	Xtrap

	stcr	r1,cr18		; 418
	bsr	Xtrap

	stcr	r1,cr18		; 419
	bsr	Xtrap

	stcr	r1,cr18		; 420
	bsr	Xtrap

	stcr	r1,cr18		; 421
	bsr	Xtrap

	stcr	r1,cr18		; 422
	bsr	Xtrap

	stcr	r1,cr18		; 423
	bsr	Xtrap

	stcr	r1,cr18		; 424
	bsr	Xtrap

	stcr	r1,cr18		; 425
	bsr	Xtrap

	stcr	r1,cr18		; 426
	bsr	Xtrap

	stcr	r1,cr18		; 427
	bsr	Xtrap

	stcr	r1,cr18		; 428
	bsr	Xtrap

	stcr	r1,cr18		; 429
	bsr	Xtrap

	stcr	r1,cr18		; 430
	bsr	Xtrap

	stcr	r1,cr18		; 431
	bsr	Xtrap

	stcr	r1,cr18		; 432
	bsr	Xtrap

	stcr	r1,cr18		; 433
	bsr	Xtrap

	stcr	r1,cr18		; 434
	bsr	Xtrap

	stcr	r1,cr18		; 435
	bsr	Xtrap

	stcr	r1,cr18		; 436
	bsr	Xtrap

	stcr	r1,cr18		; 437
	bsr	Xtrap

	stcr	r1,cr18		; 438
	bsr	Xtrap

	stcr	r1,cr18		; 439
	bsr	Xtrap

	stcr	r1,cr18		; 440
	bsr	Xtrap

	stcr	r1,cr18		; 441
	bsr	Xtrap

	stcr	r1,cr18		; 442
	bsr	Xtrap

	stcr	r1,cr18		; 443
	bsr	Xtrap

	stcr	r1,cr18		; 444
	bsr	Xtrap

	stcr	r1,cr18		; 445
	bsr	Xtrap

	stcr	r1,cr18		; 446
	bsr	Xtrap

	stcr	r1,cr18		; 447
	bsr	Xtrap

	stcr	r1,cr18		; 448 sigret
	br	bcssigret

	stcr	r1,cr18		; 449 sys_local
	br	bcssys_local

	stcr	r1,cr18		; 450 system calls
	br	bcssyscall

	stcr	r1,cr18		; 451
	bsr	Xtrap

	stcr	r1,cr18		; 452
	bsr	Xtrap

	stcr	r1,cr18		; 453
	bsr	Xtrap

	stcr	r1,cr18		; 454
	bsr	Xtrap

	stcr	r1,cr18		; 455
	bsr	Xtrap

	stcr	r1,cr18		; 456
	bsr	Xtrap

	stcr	r1,cr18		; 457
	bsr	Xtrap

	stcr	r1,cr18		; 458
	bsr	Xtrap

	stcr	r1,cr18		; 459
	bsr	Xtrap

	stcr	r1,cr18		; 460
	bsr	Xtrap

	stcr	r1,cr18		; 461
	bsr	Xtrap

	stcr	r1,cr18		; 462
	bsr	Xtrap

	stcr	r1,cr18		; 463
	bsr	Xtrap

	stcr	r1,cr18		; 464
	bsr	Xtrap

	stcr	r1,cr18		; 465
	bsr	Xtrap

	stcr	r1,cr18		; 466
	bsr	Xtrap

	stcr	r1,cr18		; 467
	bsr	Xtrap

	stcr	r1,cr18		; 468
	bsr	Xtrap

	stcr	r1,cr18		; 469
	bsr	Xtrap

	stcr	r1,cr18		; 470
	bsr	Xtrap

	stcr	r1,cr18		; 471
	bsr	Xtrap

	stcr	r1,cr18		; 472
	bsr	Xtrap

	stcr	r1,cr18		; 473
	bsr	Xtrap

	stcr	r1,cr18		; 474
	bsr	Xtrap

	stcr	r1,cr18		; 475
	bsr	Xtrap

	stcr	r1,cr18		; 476
	bsr	Xtrap

	stcr	r1,cr18		; 477
	bsr	Xtrap

	stcr	r1,cr18		; 478
	bsr	Xtrap

	stcr	r1,cr18		; 479
	bsr	Xtrap

	stcr	r1,cr18		; 480
	bsr	Xtrap

	stcr	r1,cr18		; 481
	bsr	Xtrap

	stcr	r1,cr18		; 482
	bsr	Xtrap

	stcr	r1,cr18		; 483
	bsr	Xtrap

	stcr	r1,cr18		; 484
	bsr	Xtrap

	stcr	r1,cr18		; 485
	bsr	Xtrap

	stcr	r1,cr18		; 486
	bsr	Xtrap

	stcr	r1,cr18		; 487
	bsr	Xtrap

	stcr	r1,cr18		; 488
	bsr	Xtrap

	stcr	r1,cr18		; 489
	bsr	Xtrap

	stcr	r1,cr18		; 490
	bsr	Xtrap

	stcr	r1,cr18		; 491
	bsr	Xtrap

	stcr	r1,cr18		; 492
	bsr	Xtrap

	stcr	r1,cr18		; 493
	bsr	Xtrap

	stcr	r1,cr18		; 494
	bsr	Xtrap

	stcr	r1,cr18		; 495
	bsr	Xtrap

	stcr	r1,cr18		; 496
	bsr	Xtrap

	stcr	r1,cr18		; 497
	bsr	Xtrap

	stcr	r1,cr18		; 498
	bsr	Xtrap

	stcr	r1,cr18		; 499
	bsr	Xtrap

	stcr	r1,cr18		; 500
	bsr	Xtrap

	stcr	r1,cr18		; 501
	bsr	Xtrap

	stcr	r1,cr18		; 502
	bsr	Xtrap

	stcr	r1,cr18		; 503 divz problem workaround
	bsr	Xtrap

	stcr	r1,cr18		; 504
	bsr	Xtrap

	stcr	r1,cr18		; 505
	bsr	Xtrap

	stcr	r1,cr18		; 506
	bsr	Xtrap

	stcr	r1,cr18		; 507
	bsr	Xtrap

	stcr	r1,cr18		; 508
	bsr	Xtrap

	stcr	r1,cr18		; 509
	bsr	Xtrap

	stcr	r1,cr18		; 510
	bsr	Xtrap

	stcr	r1,cr18		; 511
	bsr	Xtrap
