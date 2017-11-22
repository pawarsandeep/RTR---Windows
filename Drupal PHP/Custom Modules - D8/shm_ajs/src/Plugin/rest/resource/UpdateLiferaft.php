<?php
/**
 * Created by PhpStorm.
 * User: Piyusha Pokharana
 * Date: 10/27/2017
 * Time: 11:41 AM
 */

namespace Drupal\shm_ajs\Plugin\rest\resource;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\shm_services_entity\Entity\JobSubmission;

/**
 * Class UpdateLiferaft
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for UpdateLiferaft entity.
 *
 * @RestResource(
 *   id="UpdateLiferaftJob_rest",
 *   label= @Translation("UpdateLiferaftJob Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/updateliferaft_job"
 *   }
 * )
 */

class UpdateLiferaft extends ResourceBase{

    public function patch(array $data=[]){

        $cyl_tarid1 = $data['jobData']['gas_cylinders'];
        if($cyl_tarid1)
        {
            foreach ($cyl_tarid1 as $gascylinde) {
                if ($gascylinde['delete_flag'] == true) {
                    $gas_cylinder_node_info = JobSubmission::load($gascylinde['gas_cylinder_job_id']);
                    $gas_cylinder_node_info->delete();
                    drupal_set_message("Deleted Gas Cylinder Job Successfully.");

                }elseif ($gascylinde['gas_cylinder_job_id']) {
                    $gas_cylinder_node_info = JobSubmission::load($gascylinde['gas_cylinder_job_id']);
                    $gas_cylinder_node_info->field_gas_cylinder_lpt_date = substr($gascylinde['lpt'], 0, 10);
                    $gas_cylinder_node_info->field_gas_cylinder_ptest_request = $gascylinde['ptest_req'];
                    $gas_cylinder_node_info->field_gas_cylinde_refill_request = $gascylinde['refill_req'];
                    $gas_cylinder_node_info->field_liferaft_actual_full_weigt = $gascylinde['actual_full_weight'];
                    $gas_cylinder_node_info->field_liferaft_cylinder_sn = $gascylinde['cylinder_sn'];
                    $gas_cylinder_node_info->field_liferaft_full_weight_kgs = $gascylinde['full_weight'];
                    $gas_cylinder_node_info->field_liferaft_gas_weight_co2_kg = $gascylinde['gas_weight_co2'];
                    $gas_cylinder_node_info->field_liferaft_gas_weight_n2_kgs = $gascylinde['gas_weight_n2'];
                    $gas_cylinder_node_info->field_liferaft_tare_weight_kgs = $gascylinde['tare_weight'];
                    $gas_cylinder_node_info->save();
                    $gas_cylinder_job_id[] = $gas_cylinder_node_info->jid->value;
                }
                elseif($gascylinde['gas_cylinder_job_id'] == '') {

                    $gas_cylinder = JobSubmission::create([
                        'type' => 'liferaft_gas_cylinder_details',
                        'field_liferaft_cylinder_sn' => $gascylinde['cylinder_sn'],
                        'field_liferaft_full_weight_kgs' => $gascylinde['full_weight'],
                        'field_liferaft_tare_weight_kgs' => $gascylinde['tare_weight'],
                        'field_liferaft_gas_weight_n2_kgs' =>$gascylinde['gas_weight_n2'],
                        'field_liferaft_gas_weight_co2_kg' =>$gascylinde['gas_weight_co2'],
                        'field_liferaft_actual_full_weigt' =>$gascylinde['actual_full_weight'],
                        'field_gas_cylinder_lpt_date' => substr($gascylinde['lpt'], 0, 10),
                        'field_gas_cylinde_refill_request' => $gascylinde['refill_req'],
                        'field_gas_cylinder_ptest_request' => $gascylinde['ptest_req'],
                    ]);
                    $gas_cylinder->save();
                    $gas_cylinder_job_id[] = $gas_cylinder->jid->value;
                }
            }
        }

        $lifed_equip = $data['jobData']['lifed_equipments'];
        if($lifed_equip)
        {
            foreach ($lifed_equip as $lifed_equipments) {
                if ($lifed_equipments['delete_flag'] == true) {
                    $lifed_equipments_info = JobSubmission::load($lifed_equipments['field_lifted_id']);
                    $lifed_equipments_info->delete();
                    drupal_set_message("Deleted Lifed Item Successfully.");

                }elseif ($lifed_equipments['field_lifted_id']) {
                    $lifed_equipments_info = JobSubmission::load($lifed_equipments['field_lifted_id']);
                    $lifed_equipments_info->field_liferaft_expiry = substr($lifed_equipments['field_liferaft_expiry'], 0, 10);
                    $lifed_equipments_info->field_liferaft_lifed_items = $lifed_equipments['field_liferaft_lifed_items'];
                    $lifed_equipments_info->field_liferaft_lot_no = $lifed_equipments['field_liferaft_lot_no'];
                    $lifed_equipments_info->field_liferaft_quantity = $lifed_equipments['field_liferaft_quantity'];
                    $lifed_equipments_info->field_liferaft_replaced_expiry = substr($lifed_equipments['field_liferaft_replaced_expiry'], 0, 10);
                    $lifed_equipments_info->field_liferaft_replaced_lot_no = $lifed_equipments['field_liferaft_replaced_lot_no'];
                    $lifed_equipments_info->field_liferaft_replaced_quantity = $lifed_equipments['field_liferaft_replaced_quantity'];
                    $lifed_equipments_info->field_lr_manufacturer = $lifed_equipments['field_lr_manufacturer'];
                    $lifed_equipments_info->field_lr_replaced_manufacturer = $lifed_equipments['field_lr_replaced_manufacturer'];
                    $lifed_equipments_info->save();
                    $lifed_equipments_id[] = $lifed_equipments_info->jid->value;
                }
                elseif($lifed_equipments['field_lifted_id'] == '') {
                    $entityLifedEquip = JobSubmission::create(['type'=>'liferaft_lifed_equipments',
                        'field_liferaft_expiry'=>substr($lifed_equipments['field_liferaft_expiry'], 0, 10),
                        'field_liferaft_lifed_items'=>$lifed_equipments['field_liferaft_lifed_items'],
                        'field_liferaft_lot_no'=>$lifed_equipments['field_liferaft_lot_no'],
                        'field_lr_manufacturer'=>$lifed_equipments['field_lr_manufacturer'],
                        'field_liferaft_quantity'=>$lifed_equipments['field_liferaft_quantity'],
                        'field_liferaft_replaced_expiry'=>substr($lifed_equipments['field_liferaft_replaced_expiry'], 0, 10),
                        'field_liferaft_replaced_lot_no'=>$lifed_equipments['field_liferaft_replaced_lot_no'],
                        'field_lr_replaced_manufacturer'=>$lifed_equipments['field_lr_replaced_manufacturer'],
                        'field_liferaft_replaced_quantity'=>$lifed_equipments['field_liferaft_replaced_quantity']
                    ]);
                    $entityLifedEquip->save();
                    $lifed_equipments_id[] = $entityLifedEquip->id();
                }
            }
        }
        if($data['jobData']['modeartion_state'] == 'draft')
        {
            $moderation_state = 'draft';
        }elseif ($data['jobData']['modeartion_state'] == 'pending_approval')
        {
            $moderation_state = 'pending_approval';

        }elseif ($data['jobData']['modeartion_state'] == 'approved')
        {
            $moderation_state = 'approved';
        }
        if($data['entity_id']) {
            $reinspection_job_info = JobSubmission::load($data['entity_id']);
            $reinspection_job_info->field_liferaft_agas_cylinder = $gas_cylinder_job_id;
            $reinspection_job_info->field_liferaft_lifed_equipments = $lifed_equipments_id;
            $reinspection_job_info->field_lr_uc_stretch_time_off = $data['jobData']['field_lr_uc_stretch_time_off'];
            $reinspection_job_info->field_lr_uc_stretch_time_on= $data['jobData']['field_lr_uc_stretch_time_on'];
            $reinspection_job_info->field_lr_uc_final_time_off= $data['jobData']['field_lr_uc_final_time_off'];
            $reinspection_job_info->field_lr_uc_final_time_on= $data['jobData']['field_lr_uc_final_time_on'];
            $reinspection_job_info->field_lr_uc_reading_off= $data['jobData']['field_lr_uc_reading_off'];
            $reinspection_job_info->field_lr_uc_reading_on= $data['jobData']['field_lr_uc_reading_on'];
            $reinspection_job_info->field_lr_uc_stretch_time_on= $data['jobData']['field_lr_uc_stretch_time_on'];
            $reinspection_job_info->field_lr_uc_temp_off= $data['jobData']['field_lr_uc_temp_off'];
            $reinspection_job_info->field_lr_rv_blow_off_uc= $data['jobData']['field_lr_rv_blow_off_uc'];
            $reinspection_job_info->field_lr_rv_reset_lc= $data['jobData']['field_lr_rv_reset_lc'];
            $reinspection_job_info->field_lr_rv_blow_off_lc= $data['jobData']['field_lr_rv_blow_off_lc'];
            $reinspection_job_info->field_lr_rv_reset_uc= $data['jobData']['field_lr_rv_reset_uc'];

            $reinspection_job_info->field_liferaft_certificate_type = $data['jobData']['field_liferaft_certificate_type'];

            $reinspection_job_info->field_lr_lc_final_time_off= $data['jobData']['field_lr_lc_final_time_off'];
            $reinspection_job_info->field_lr_lc_final_time_on= $data['jobData']['field_lr_lc_final_time_on'];
            $reinspection_job_info->field_lr_lc_reading_off= $data['jobData']['field_lr_lc_reading_off'];
            $reinspection_job_info->field_lr_lc_reading_on= $data['jobData']['field_lr_lc_reading_on'];
            $reinspection_job_info->field_lr_lc_stretch_time_off= $data['jobData']['field_lr_lc_stretch_time_off'];
            $reinspection_job_info->field_lr_lc_stretch_time_on= $data['jobData']['field_lr_lc_stretch_time_on'];
            $reinspection_job_info->field_lr_lc_temp_off= $data['jobData']['field_lr_lc_temp_off'];
            $reinspection_job_info->field_lr_lc_temp_on= $data['jobData']['field_lr_lc_temp_on'];

            $reinspection_job_info->field_lr_floor_final_time_off= $data['jobData']['field_lr_floor_final_time_off'];
            $reinspection_job_info->field_lr_floor_final_time_on= $data['jobData']['field_lr_floor_final_time_on'];
            $reinspection_job_info->field_lr_floor_stretch_time_off= $data['jobData']['field_lr_floor_stretch_time_off'];
            $reinspection_job_info->field_lr_floor_stretch_time_on= $data['jobData']['field_lr_floor_stretch_time_on'];
            $reinspection_job_info->field_lr_floor_temp_off= $data['jobData']['field_lr_floor_temp_off'];
            $reinspection_job_info->field_lr_floor_temp_on= $data['jobData']['field_lr_floor_temp_on'];
            $reinspection_job_info->field_lr_floor_reading_off= $data['jobData']['field_lr_floor_reading_off'];
            $reinspection_job_info->field_lr_floor_reading_on= $data['jobData']['field_lr_floor_reading_on'];


            /*---------------------Containers----------------------*/
            $reinspection_job_info->field_lr_container_seal_gasket== $data['jobData']['field_lr_container_seal_gasket'];
            $reinspection_job_info->field_lr_container_sealing_strip= $data['jobData']['field_lr_container_sealing_strip'];
            $reinspection_job_info->field_lr_container_marking_label= $data['jobData']['field_lr_container_marking_label'];
            $reinspection_job_info->field_lr_f_r_p_container= $data['jobData']['field_lr_f_r_p_container'];
            $reinspection_job_info->field_lr_container_container_id= $data['jobData']['field_lr_container_container_id'];
            $reinspection_job_info->field_lr_container_breaking_band = $data['jobData']['field_lr_container_breaking_band'];
            /*---------------------INFLATION/DEFLATION ITEMS----------------------*/
            $reinspection_job_info->field_lr_ie_operating_head = $data['jobData']['field_lr_ie_operating_head'];
            $reinspection_job_info->field_lr_ie_high_pressure_hose = $data['jobData']['field_lr_ie_high_pressure_hose'];
            $reinspection_job_info->field_lr_ie_t_y_connectors = $data['jobData']['field_lr_ie_t_y_connectors'];
            $reinspection_job_info->field_lr_ie_infl_defl_valves = $data['jobData']['field_lr_ie_infl_defl_valves'];
            $reinspection_job_info->field_lr_ie_rubberized_bellow = $data['jobData']['field_lr_ie_rubberized_bellow'];
            $reinspection_job_info->field_lr_ie_painter_cord = $data['jobData']['field_lr_ie_painter_cord'];
            $reinspection_job_info->field_lr_ie_relief_valves = $data['jobData']['field_lr_ie_relief_valves'];
            $reinspection_job_info->field_lr_lc_infl_defl_valve = $data['jobData']['field_lr_lc_infl_defl_valve'];
            $reinspection_job_info->field_lr_lc_relief_valve = $data['jobData']['field_lr_lc_relief_valve'];
            $reinspection_job_info->field_lr_f_r_p_container = $data['jobData']['field_lr_f_r_p_container'];
            $reinspection_job_info->field_lr_uc_relief_valve = $data['jobData']['field_lr_uc_relief_valve'];
            /*---------------------Liferaft----------------------*/

            $reinspection_job_info->field_lr_boarding_ladder = $data['jobData']['field_lr_boarding_ladder'];
            $reinspection_job_info->field_lr_boarding_ramp = $data['jobData']['field_lr_boarding_ramp'];
            $reinspection_job_info->field_lr_lifelines = $data['jobData']['field_lr_lifelines'];
            $reinspection_job_info->field_lr_liferafts_fabrics = $data['jobData']['field_lr_liferafts_fabrics'];
            $reinspection_job_info->field_lr_water_pockets = $data['jobData']['field_lr_water_pockets'];
            $reinspection_job_info->field_lr_zips_draw_cords = $data['jobData']['field_lr_zips_draw_cords'];
            $reinspection_job_info->field_lr_lower_chamber = $data['jobData']['field_lr_lower_chamber'];
            $reinspection_job_info->field_lr_upper_chamber = $data['jobData']['field_lr_upper_chamber'];
            $reinspection_job_info->field_lr_floor_reeds_seams = $data['jobData']['field_lr_floor_reeds_seams'];
            $reinspection_job_info->field_lr_strut_arches_seams = $data['jobData']['field_lr_strut_arches_seams'];
            $reinspection_job_info->field_lr_outer_light = $data['jobData']['field_lr_outer_light'];
            $reinspection_job_info->field_lr_inner_canopy = $data['jobData']['field_lr_inner_canopy'];
            $reinspection_job_info->field_lr_outer_canopy = $data['jobData']['field_lr_outer_canopy'];
            $reinspection_job_info->field_lr_sp_r_reflective_tape = $data['jobData']['field_lr_sp_r_reflective_tape'];
            /*--------------------SURVIVAL PACK---------------------------------*/
            $reinspection_job_info->field_lr_sp_adhesive_tube = $data['jobData']['field_lr_sp_adhesive_tube'];
            $reinspection_job_info->field_lr_sp_bailers = $data['jobData']['field_lr_sp_bailers'];
            $reinspection_job_info->field_lr_sp_bellow = $data['jobData']['field_lr_sp_bellow'];
            $reinspection_job_info->field_lr_sp_fishing_kit = $data['jobData']['field_lr_sp_fishing_kit'];
            $reinspection_job_info->field_lr_sp_floating_knife = $data['jobData']['field_lr_sp_floating_knife'];
            $reinspection_job_info->field_lr_sp_heliograph = $data['jobData']['field_lr_sp_heliograph'];
            $reinspection_job_info->field_lr_sp_inner_light = $data['jobData']['field_lr_sp_inner_light'];
            $reinspection_job_info->field_lr_sp_leak_stoppers = $data['jobData']['field_lr_sp_leak_stoppers'];
            $reinspection_job_info->field_lr_sp_paddles = $data['jobData']['field_lr_sp_paddles'];
            $reinspection_job_info->field_lr_sp_polythene_liner = $data['jobData']['field_lr_sp_polythene_liner'];

            $reinspection_job_info->field_lr_sp_quoits_rescue_line = $data['jobData']['field_lr_sp_quoits_rescue_line'];
            $reinspection_job_info->field_lr_sp_r_reflective_tape = $data['jobData']['field_lr_sp_r_reflective_tape'];
            $reinspection_job_info->field_lr_sp_radar_reflector = $data['jobData']['field_lr_sp_radar_reflector'];
            $reinspection_job_info->field_lr_sp_repair_kit = $data['jobData']['field_lr_sp_repair_kit'];
            $reinspection_job_info->field_lr_sp_sea_anchor = $data['jobData']['field_lr_sp_sea_anchor'];
            $reinspection_job_info->field_lr_sp_sickness_bags = $data['jobData']['field_lr_sp_sickness_bags'];
            $reinspection_job_info->field_lr_sp_signaling_torch = $data['jobData']['field_lr_sp_signaling_torch'];
            $reinspection_job_info->field_lr_sp_spare_bulb = $data['jobData']['field_lr_sp_spare_bulb'];
            $reinspection_job_info->field_lr_sp_sponges = $data['jobData']['field_lr_sp_sponges'];
            $reinspection_job_info->field_lr_sp_survival_book = $data['jobData']['field_lr_sp_survival_book'];

            $reinspection_job_info->field_lr_sp_t_p_a_ = $data['jobData']['field_lr_sp_t_p_a_'];
            $reinspection_job_info->field_lr_sp_tin_openers = $data['jobData']['field_lr_sp_tin_openers'];
            $reinspection_job_info->field_lr_sp_torch_cells = $data['jobData']['field_lr_sp_torch_cells'];
            $reinspection_job_info->field_lr_sp_valise = $data['jobData']['field_lr_sp_valise'];
            $reinspection_job_info->field_lr_sp_whistle = $data['jobData']['field_lr_sp_whistle'];
            $reinspection_job_info->field_lr_drinking_vessel = $data['jobData']['field_lr_drinking_vessel'];

            $reinspection_job_info->field_lr_hru_expiry = substr($data['jobData']['field_lr_hru_expiry'],0,10);
            $reinspection_job_info->field_lr_hru_sn = $data['jobData']['field_lr_hru_sn'];
            $reinspection_job_info->field_lr_hru_maker = $data['jobData']['field_lr_hru_maker'];
            $reinspection_job_info->field_lr_wp_test = $data['jobData']['field_lr_wp_test'];
            $reinspection_job_info->field_lr_wp_latest_date = substr($data['jobData']['field_lr_wp_latest_date'],0,10);
            $reinspection_job_info->field_lr_nap_test = $data['jobData']['field_lr_nap_test'];
            $reinspection_job_info->field_lr_nap_latest_date = substr($data['jobData']['field_lr_nap_latest_date'],0,10);
            $reinspection_job_info->field_lr_fs_test = $data['jobData']['field_lr_fs_test'];
            $reinspection_job_info->field_lr_fs_latest_date = substr($data['jobData']['field_lr_fs_latest_date'],0,10);
            $reinspection_job_info->field_lr_gas_inflation_test = $data['jobData']['field_lr_gas_inflation_test'];
            $reinspection_job_info->field_lr_git_latest_date = substr($data['jobData']['field_lr_git_latest_date'],0,10);
            $reinspection_job_info->field_lr_load_test = $data['jobData']['field_lr_load_test'];
            $reinspection_job_info->field_lr_lt_latest_date = substr($data['jobData']['field_lr_lt_latest_date'],0,10);
            $reinspection_job_info->field_lr_weight = $data['jobData']['field_lr_weight'];
            $reinspection_job_info->field_lr_painter_line_length_m_ = $data['jobData']['field_lr_painter_line_length_m_'];
            $reinspection_job_info->field_lr_stowage_height_m_ = $data['jobData']['field_lr_stowage_height_m_'];
            $reinspection_job_info->field_lr_class_inspection = $data['jobData']['field_lr_class_inspection'];
            $reinspection_job_info->field_lr_mmd_inspection = $data['jobData']['field_lr_mmd_inspection'];
            $reinspection_job_info->field_reinspection_comments = $data['jobData']['field_reinspection_comments'];
            $reinspection_job_info->field_service_engineer = $data['jobData']['field_service_engineer']['uid'];
            $reinspection_job_info->moderation_state = $moderation_state;

            $reinspection_job_info->save();
        }

        drupal_set_message("Updated Job Successfully.");

        $response = ['message' => 'Get liferaft test'];
        return new ResourceResponse($response);
    }
}