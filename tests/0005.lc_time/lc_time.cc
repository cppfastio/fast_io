#include<fast_io.h>
#include<fast_io_i18n.h>
#include<fast_io_device.h>

int main()
{
	fast_io::posix_tzset();
	fast_io::obuf_file obf("lc_time.txt");
	constexpr char const* files[]{ "aa_DJ","aa_ER","aa_ER@saaho","aa_ET","af_ZA","agr_PE","ak_GH","am_ET","anp_IN","an_ES","ar_AE","ar_BH","ar_DZ","ar_EG","ar_IN","ar_IQ","ar_JO","ar_KW","ar_LB","ar_LY","ar_MA","ar_OM","ar_QA","ar_SA","ar_SD","ar_SS","ar_SY","ar_TN","ar_YE","ast_ES","as_IN","ayc_PE","az_AZ","az_IR","bem_ZM","ber_DZ","ber_MA","be_BY","be_BY@latin","bg_BG","bhb_IN","bho_IN","bho_NP","bi_VU","bn_BD","bn_IN","bo_CN","bo_IN","brx_IN","br_FR","br_FR@euro","bs_BA","byn_ER","C","ca_AD","ca_ES","ca_ES@euro","ca_ES@valencia","ca_FR","ca_IT","ce_RU","chr_US","ckb_IQ","cmn_TW","crh_UA","csb_PL","cs_CZ","cv_RU","cy_GB","da_DK","de_AT","de_AT@euro","de_BE","de_BE@euro","de_CH","de_DE","de_DE@euro","de_IT","de_LI","de_LU","de_LU@euro","doi_IN","dsb_DE","dv_MV","dz_BT","el_CY","el_GR","el_GR@euro","en_AG","en_AU","en_BW","en_CA","en_DK","en_GB","en_HK","en_IE","en_IE@euro","en_IL","en_IN","en_NG","en_NZ","en_PH","en_SC","en_SG","en_US","en_ZA","en_ZM","en_ZW","eo","es_AR","es_BO","es_CL","es_CO","es_CR","es_CU","es_DO","es_EC","es_ES","es_ES@euro","es_GT","es_HN","es_MX","es_NI","es_PA","es_PE","es_PR","es_PY","es_SV","es_US","es_UY","es_VE","et_EE","eu_ES","eu_ES@euro","fa_IR","ff_SN","fil_PH","fi_FI","fi_FI@euro","fo_FO","fr_BE","fr_BE@euro","fr_CA","fr_CH","fr_FR","fr_FR@euro","fr_LU","fr_LU@euro","fur_IT","fy_DE","fy_NL","ga_IE","ga_IE@euro","gd_GB","gez_ER","gez_ER@abegede","gez_ET","gez_ET@abegede","gl_ES","gl_ES@euro","gu_IN","gv_GB","hak_TW","ha_NG","he_IL","hif_FJ","hi_IN","hne_IN","hr_HR","hsb_DE","ht_HT","hu_HU","hy_AM","i18n","ia_FR","id_ID","ig_NG","ik_CA","is_IS","it_CH","it_IT","it_IT@euro","iu_CA","ja_JP","kab_DZ","ka_GE","kk_KZ","kl_GL","km_KH","kn_IN","kok_IN","ko_KR","ks_IN","ks_IN@devanagari","ku_TR","kw_GB","ky_KG","lb_LU","lg_UG","lij_IT","li_BE","li_NL","ln_CD","lo_LA","lt_LT","lv_LV","lzh_TW","mag_IN","mai_IN","mai_NP","mfe_MU","mg_MG","mhr_RU","miq_NI","mi_NZ","mjw_IN","mk_MK","ml_IN","mni_IN","mnw_MM","mn_MN","mr_IN","ms_MY","mt_MT","my_MM","nan_TW","nan_TW@latin","nb_NO","nds_DE","nds_NL","ne_NP","nhn_MX","niu_NU","niu_NZ","nl_AW","nl_BE","nl_BE@euro","nl_NL","nl_NL@euro","nn_NO","nr_ZA","nso_ZA","oc_FR","om_ET","om_KE","or_IN","os_RU","pap_AW","pap_CW","pa_IN","pa_PK","pl_PL","POSIX","ps_AF","pt_BR","pt_PT","pt_PT@euro","quz_PE","raj_IN","ro_RO","ru_RU","ru_UA","rw_RW","sah_RU","sat_IN","sa_IN","sc_IT","sd_IN","sd_IN@devanagari","se_NO","sgs_LT","shn_MM","shs_CA","sid_ET","si_LK","sk_SK","sl_SI","sm_WS","so_DJ","so_ET","so_KE","so_SO","sq_AL","sq_MK","sr_ME","sr_RS","sr_RS@latin","ss_ZA","st_ZA","sv_FI","sv_FI@euro","sv_SE","sw_KE","sw_TZ","szl_PL","ta_IN","ta_LK","tcy_IN","te_IN","tg_TJ","the_NP","th_TH","tig_ER","ti_ER","ti_ET","tk_TM","tl_PH","tn_ZA","to_TO","tpi_PG","tr_CY","tr_TR","ts_ZA","tt_RU","tt_RU@iqtelif","ug_CN","uk_UA","unm_US","ur_IN","ur_PK","uz_UZ","uz_UZ@cyrillic","ve_ZA","vi_VN","wae_CH","wal_ET","wa_BE","wa_BE@euro","wo_SN","xh_ZA","yi_US","yo_NG","yue_HK","yuw_PG","zh_CN","zh_HK","zh_SG","zh_TW","zu_ZA" };
	using namespace fast_io::mnp;
	for (auto fnm : files)
	{
		fast_io::l10n loc(fnm);
		auto v{posix_clock_gettime(fast_io::posix_clock_id::realtime)};
		fast_io::iso8601_timestamp utc_tsp{ utc(v) };
		fast_io::iso8601_timestamp tsp{ local(v) };
		print(imbue(loc, obf), chvw(fnm),"\n"
			"unix:\t", v,"\n"
			"utc\niso 8601:\t", utc_tsp, "\n"
			"d_t_fmt:\t", d_t_fmt(utc_tsp), "\n"
			"d_fmt:\t", d_fmt(utc_tsp), "\n"
			"t_fmt:\t", t_fmt(utc_tsp), "\n"
			"t_fmt_ampm:\t", t_fmt_ampm(utc_tsp), "\n"
			"t_fmt:\t", t_fmt(utc_tsp), "\n"
			"date_fmt:\t", date_fmt(utc_tsp), "\n"
			"era_d_t_fmt:\t", era_d_t_fmt(utc_tsp), "\n"
			"era_d_fmt:\t", era_d_fmt(utc_tsp), "\n"
			"era_t_fmt:\t", era_t_fmt(utc_tsp), "\n"
			"local\niso 8601:\t", tsp, "\n"
			"d_t_fmt:\t", d_t_fmt(tsp), "\n"
			"d_fmt:\t", d_fmt(tsp), "\n"
			"t_fmt:\t", t_fmt(tsp), "\n"
			"t_fmt_ampm:\t", t_fmt_ampm(tsp), "\n"
			"t_fmt:\t", t_fmt(tsp), "\n"
			"date_fmt:\t", date_fmt(tsp), "\n"
			"era_d_t_fmt:\t", era_d_t_fmt(tsp), "\n"
			"era_d_fmt:\t", era_d_fmt(tsp), "\n"
			"era_t_fmt:\t", era_t_fmt(tsp), "\n"
			"-----------------------------------------------------------------------------\n");
	}
}