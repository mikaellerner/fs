*
* Copyright (c) 2020 NVI, Inc.
*
* This file is part of VLBI Field System
* (see http://github.com/nvi-inc/fs).
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
c fscom_quikr.i
c
c See fscom.i for information on the structure
c
c
      REAL ARR1FX_FS(6), ARR2FX_FS(6), ARR3FX_FS(6),
     .     ARR4FX_FS(6), ARR5FX_FS(6), ARR6FX_FS(6),
     . BEAMSZ_FS(6), BM1NF_FS,
     . BM2NF_FS, BMFP_FS, CAL1NF, CAL2NF,
     . CALFP, CALTMP(6),
     . COR1FX_FS, COR2FX_FS, COR3FX_FS,
     . COR4FX_FS, COR5FX_FS, COR6FX_FS,
     . CTOFNF, FASTFW(2,2), FASTRV(2,2), FIWO_FS(2,2),
     . FLX1FX_FS, FLX2FX_FS, FLX3FX_FS, FLX4FX_FS,
     . FLX5FX_FS, FLX6FX_FS,
     . FOWO_FS(2,2), 
     . FX1NF_FS, FX2NF_FS, FXFP_FS,
     . LMTN_FS(2,2), LMTP_FS(2,2), PETHR(2),
     . RBDread_FS(2), 
     . RBDwrite_FS(2), RNGLC_FS(2), RSread_FS(2), RSwrite_FS(2),
     . RV13_FS(2), RV15flip_FS(2), RV15for_FS(2), RV15rev_FS(2),
     . RV15scale_FS(2), RVrevW_FS(2), RVw0_FS(2), RVw8_FS(2),
     . Rsdread_FS(2), Rsdwrite_FS(2), SIWO_FS(2,2), SOWO_FS(2,2),
     $     STEPFP, STEPLC_FS(2), STEPNF, TP1IFD, TP2IFD,
     . TPERER(2), TPSOR(31), TPSPC(31), TPZERO(31), VADCRX,
     . VADCST, VLTPK_FS(2), VMINPK_FS(2),
     . SLOWFW(2,2),SLOWRV(2,2),
     . tpidiff(MAX_DET),caltemps(MAX_DET),SSIZFP

      INTEGER IADCRX, IADCST, IATLVC(15),
     . IATUVC(15), IBUGPC, IBWTAP(2), IBXHRX,
     . IBXHST, IBYPAS(2), IBR4TAP(2), IBYPPC, ICHAND,
     . ICHPER(2), ICLWO_FS(2), IDCHRX, IDCHST, IDECPA_FS(2),
     . IEQTAP(2), IEQ4TAP(2), IERRDC_FS, IFAMRX(3),
     . IFAMST(3), IHDLC_FS(2),
     . IHDPK_FS(2), IHDWO_FS(2), ILOHST, ILOWTP(2),
     . IMDL1FX_FS, IMDL2FX_FS, IMDL3FX_FS,
     . IMDL4FX_FS, IMDL5FX_FS, IMDL6FX_FS,
     . IMODDC, IMODPE(2),
     . INPFM, INSPER(2), INTPFP, INTPNF, IOL1IF_FS,
     . IOL2IF_FS, IPAUPC,
     . IREMIF, IREMVC(15), IREPPC, IRSTTP(2), ISETHR(2),
     . ISYNFM, ITERPK_FS(2), ITRAKAUS_FS(2),
     . ITRAKBUS_FS(2), ITRKEN(28,2),
     . ITRKENUS_FS(28,2), ITRKPA(2,2),
     . ITRKPC(28), ITRPER(2), LOSTRX, LOSTST, LSWCAL,
     . LTRKEN(4),
     . NBLKPC, NCYCPC, NPTSFP, NREPFP, NREPNF, 
     . NSAMPLC_FS(2), NSAMPPK_FS(2),
     . ICHFP_FS, ICH1NF_FS, ICH2NF_FS,
     . iolif3_fs,
     . itapof(200,2),IWTFP,
     . b_quikr(INT_ALIGN),e_quikr

      LOGICAL KVrevW_FS(2), KV15rev_FS(2), KV15for_FS(2),
     $     KV15scale_FS(2), KV13_FS(2), KV15flip_FS(2),
     $     KVw0_FS(2), KVw8_FS(2), KSREAD_FS(2),
     .     KSwrite_FS(2), Ksdread_FS(2), Ksdwrite_FS(2), KBDwrite_FS(2),
     $     KBDread_FS(2), KHECHO_FS, KPEAKV_FS(2),KPOSHD_FS(2,2),
     .     KRDWO_FS(2), KWRWO_FS(2), KDOAUX_FS(2), KRPTP_FS(2),
     $     KMVTP_FS(2), KENTP_FS(2), KAUTOHD_FS(2), KLDTP_FS(2)

      INTEGER*2 LAUXFM(6), LAUXFM4(4), LAXFP(2), LDEVFP(2), LDV1NF, 
     . LDV2NF, LTPCHK(2,2), LTPNUM(4,2), LOPRID(6), QFILL(2)

      common/fscom_quikr/b_quikr,
     . ARR1FX_FS, ARR2FX_FS, ARR3FX_FS,
     . ARR4FX_FS, ARR5FX_FS, ARR6FX_FS,
     . BEAMSZ_FS, BM1NF_FS, BM2NF_FS, BMFP_FS, CAL1NF, CAL2NF,
     . CALFP, CALTMP,
     . COR1FX_FS, COR2FX_FS, COR3FX_FS,
     . COR4FX_FS, COR5FX_FS, COR6FX_FS,
     . CTOFNF, FASTFW, FASTRV, FIWO_FS,
     . FLX1FX_FS, FLX2FX_FS, FLX3FX_FS, FLX4FX_FS,
     . FLX5FX_FS, FLX6FX_FS,
     . FOWO_FS,
     . FX1NF_FS, FX2NF_FS, FXFP_FS,
     . LMTN_FS, LMTP_FS, PETHR,
     . RBDread_FS,
     . RBDwrite_FS, RNGLC_FS, Rsdread_FS, Rsdwrite_FS,
     . RSread_FS, RSwrite_FS, RV13_FS, RV15flip_FS, 
     . RV15for_FS, RV15rev_FS, RV15scale_FS, RVrevW_FS,
     . RVw0_FS, RVw8_FS, SIWO_FS, SOWO_FS, STEPFP,
     . STEPLC_FS, STEPNF, TP1IFD, TP2IFD,
     . TPERER, TPSOR, TPSPC, TPZERO, VADCRX,
     . VADCST, VLTPK_FS, VMINPK_FS,
     . SLOWFW,SLOWRV,
     . tpidiff,caltemps,SSIZFP,
     . IADCRX, IADCST, IATLVC,
     . IATUVC, IBUGPC, IBWTAP, IBXHRX,
     . IBXHST, IBYPAS, IBR4TAP, IBYPPC, ICHAND, 
     . ICHPER, ICLWO_FS, IDCHRX, IDCHST,
     . IDECPA_FS, IEQTAP, IEQ4TAP, IERRDC_FS, IFAMRX,
     . IFAMST, IHDLC_FS,
     . IHDPK_FS, IHDWO_FS, ILOHST, ILOWTP,
     . IMDL1FX_FS, IMDL2FX_FS, IMDL3FX_FS,
     . IMDL4FX_FS, IMDL5FX_FS, IMDL6FX_FS,
     . IMODDC, IMODPE,
     . INPFM, INSPER, INTPFP, INTPNF, IOL1IF_FS,
     . IOL2IF_FS, IPAUPC,
     . IREMIF, IREMVC, IREPPC, IRSTTP, ISETHR,
     . ISYNFM, ITERPK_FS,
     . ITRAKAUS_FS, ITRAKBUS_FS, ITRKEN,
     . ITRKENUS_FS, ITRKPA, ITRKPC, ITRPER,
     . LOSTRX, LOSTST, LSWCAL,
     . LTRKEN, 
     . NBLKPC, NCYCPC, NPTSFP, NREPFP,
     . NREPNF, NSAMPLC_FS, NSAMPPK_FS,
     . ICHFP_FS, ICH1NF_FS, ICH2NF_FS,
     . iolif3_fs, itapof,IWTFP,
     . KVrevW_FS, KV15rev_FS, KV15for_FS, KV15scale_FS,
     . KV13_FS, KV15flip_FS, KVw0_FS, KVw8_FS,
     . KSread_FS, KSwrite_FS, Ksdread_FS, Ksdwrite_FS,
     . KBDwrite_FS, KBDread_FS, KHECHO_FS, KPEAKV_FS, KPOSHD_FS,
     . KRDWO_FS, KWRWO_FS, KDOAUX_FS, KRPTP_FS, KMVTP_FS,
     . KENTP_FS, KAUTOHD_FS, KLDTP_FS,
     . LAUXFM, LAUXFM4, LAXFP, LDEVFP, LDV1NF, LDV2NF,
     . LTPCHK, LTPNUM, LOPRID,
     . qfill, e_quikr
