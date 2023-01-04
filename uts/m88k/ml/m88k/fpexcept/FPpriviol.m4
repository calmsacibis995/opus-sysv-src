;function _FPpriviol --      
;See the documentation that came with this release for an overall description
;of this code.


		include(sub.m4)  ;include macros
		global _FPpriviol
		text
_FPpriviol:
		sub	r3,r0,1
		jmp.n	r1
		or	r4,r0,0x20
