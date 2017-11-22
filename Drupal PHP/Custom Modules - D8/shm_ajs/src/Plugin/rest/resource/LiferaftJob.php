<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 9/26/2017
 * Time: 2:01 AM
 */

namespace Drupal\shm_ajs\Plugin\rest\resource;

use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\shm_services_entity\Entity\JobSubmission;
use Drupal\node\Entity\Node;

/**
 * Class Liferaft
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for Liferaft entity.
 *
 * @RestResource(
 *   id="liferaft_job_rest",
 *   label= @Translation("Liferaft Job Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/liferaft-job",
 *    "https://www.drupal.org/link-relations/create" = "/rest_api/liferaft-job"
 *   }
 * )
 */
class LiferaftJob extends ResourceBase
{
  public function get(){
    $response = ['message' => 'Get liferaft test'];
    return new ResourceResponse($response);
  }

  public function post(array $data=[]){
    $liferaftID =NULL;
    $gasCylinders = array();
    $lifedEquip = array();
    if ($data['liferaft']['isNew'])
    {
      // Code to create new Liferaft entity for submitted job.
      $liferaft = Node::create(['type'=>'liferafts',
		'title' => $data['liferaft']['field_liferaft_serial_no'],
        'field_last_surveyed_on'=>substr($data['liferaft']['field_liferaft_previous_inspec'],0,10),
        'field_manufacturer'=>$data['liferaft']['field_liferaft_manufacturer']['nid'],
        'field_manufacturing_date'=>substr($data['liferaft']['field_liferaft_date_manufacturing'],0,10),
        'field_operating_mechanism'=>$data['liferaft']['field_operating_mechanism'],
        'field_lr_solas_pack_type'=>$data['liferaft']['field_liferaft_survivalpack_type'],
        'field_surveyed_on'=>substr($data['liferaft']['field_liferaft_date_inspection'],0,10),
        'field_type_capacity'=>$data['liferaft']['field_liferaft_type']['tid'],
        'field_vessel'=>$data['liferaft']['vessel']['nid'],
		'field_liferaft_model' => $data['liferaft']['field_liferaft_model'],
      ]);
      $liferaft->save();
      $liferaftID = $liferaft->id();
    }
    else {
      // Liferaft already exists
      $liferaftID = $data['liferaft']['liferaft_id'];
      if($liferaftID) {
          $liferaft = Node::load($liferaftID);
          $liferaft->set('field_last_surveyed_on', substr($data['liferaft']['field_liferaft_previous_inspec'], 0, 10));
          $liferaft->set('field_surveyed_on', substr($data['liferaft']['field_liferaft_date_inspection'], 0, 10));
          $liferaft->save();
      }
    }
    if($liferaftID) {
        foreach ($data['gas_cylinders'] as $gasCylinder) {
            $entityGasCylinder = JobSubmission::create(['type' => 'liferaft_gas_cylinder_details',
                'field_liferaft_actual_full_weigt' => $gasCylinder['actual_full_weight'],
                'field_liferaft_cylinder_sn' => $gasCylinder['cylinder_sn'],
                'field_liferaft_full_weight_kgs' => $gasCylinder['full_weight'],
                'field_liferaft_gas_weight_co2_kg' => $gasCylinder['gas_weight_co2'],
                'field_liferaft_gas_weight_n2_kgs' => $gasCylinder['gas_weight_n2'],
                'field_gas_cylinder_lpt_date' => substr($gasCylinder['lpt'], 0, 10),
                'field_liferaft_ptest_request' => $gasCylinder['ptest_req'],
                'field_liferaft_refill_request' => $gasCylinder['refill_req'],
                'field_liferaft_tare_weight_kgs' => $gasCylinder['tare_weight'],
            ]);
            $entityGasCylinder->save();
            $gasCylinders[] = $entityGasCylinder->id();
        }

        // Lifed equipments
        foreach ($data['lifed_equipments'] as $lifedEquipment) {
            $entityLifedEquip = JobSubmission::create(['type' => 'liferaft_lifed_equipments',
                'field_liferaft_expiry' => substr($lifedEquipment['field_liferaft_expiry'], 0, 10),
                'field_liferaft_lifed_items' => $lifedEquipment['field_liferaft_lifed_items'],
                'field_liferaft_lot_no' => $lifedEquipment['field_liferaft_lot_no'],
                'field_lr_manufacturer' => $lifedEquipment['field_lr_manufacturer'],
                'field_liferaft_quantity' => $lifedEquipment['field_liferaft_quantity'],
                'field_liferaft_replaced_expiry' => substr($lifedEquipment['field_liferaft_replaced_expiry'], 0, 10),
                'field_liferaft_replaced_lot_no' => $lifedEquipment['field_liferaft_replaced_lot_no'],
                'field_lr_replaced_manufacturer' => $lifedEquipment['field_lr_replaced_manufacturer'],
                'field_liferaft_replaced_quantity' => $lifedEquipment['field_liferaft_replaced_quantity']
            ]);
            $entityLifedEquip->save();
            $lifedEquip[] = $entityLifedEquip->id();
        }
		 if($data['invoice']) {
        $invoice = JobSubmission::create([
            'type' => 'invoice_liferaft',
            'field_lr_invoice_no' => $data['invoice']['field_lr_invoice_no'],
            'field_invoice_date' => substr($data['invoice']['field_invoice_date'], 0, 10),
            'field_oem_certification_charges' => $data['invoice']['field_oem_certification_charges'],
            'field_lr_inv_replacement_charges' => $data['invoice']['field_lr_inv_replacement_charges'],
            'field_lr_in_certification_charge' => $data['invoice']['field_lr_in_certification_charge'],
            'field_lr_oem_certificate_no' => $data['invoice']['field_lr_oem_certificate_no'],
            'field_lr_inv_service_charges' => $data['invoice']['field_lr_inv_service_charges'],
            'field_lr_inv_invoice_amount' => $data['invoice']['field_lr_inv_invoice_amount'],
        ]);
        $invoice->save();
        $invocieID = $invoice->id();
		 }
        if ($data['modeartion_state'] == 'draft') {
            $moderation_state = 'draft';
        } elseif ($data['modeartion_state'] == 'pending_approval') {
            $moderation_state = 'approved';
        } elseif ($data['modeartion_state'] == 'approved') {
            $moderation_state = 'approved';
        }

        $job = JobSubmission::create(['type' => 'liferaft_service_job',
            'moderation_state' => $moderation_state,
            'field_lr_sp_adhesive_tube' => $data['field_lr_sp_adhesive_tube'],
            'field_lr_sp_bailers' => $data['field_lr_sp_bailers'],
            'field_lr_sp_bellow' => $data['field_lr_sp_bellow'],
            'field_lr_boarding_ladder' => $data['field_lr_boarding_ladder'],
            'field_lr_boarding_ramp' => $data['field_lr_boarding_ramp'],
            'field_lr_container_breaking_band' => $data['field_lr_container_breaking_band'],
            'field_service_engineer' => $data['field_service_engineer']['uid'],
//      'field_liferaft_certificate_no'=>$data[''],
            'field_liferaft_certificate_type' => $data['field_liferaft_certificate_type'],
//      'field_lr_class_cert_charge'=>$data[''],
            'field_lr_class_inspection' => $data['field_lr_class_inspection'],//
            'field_lr_container_container_id' => $data['field_lr_container_container_id'],
            //'field_liferaft_date_inspection'=>substr($data['liferaft']['field_liferaft_date_inspection'])),//
            // 'field_liferaft_previous_inspec'=>substr($data['liferaft']['field_liferaft_date_inspection'])),//
            'field_lr_f_r_p_container' => $data['field_lr_f_r_p_container'],
            'field_lr_sp_fishing_kit' => $data['field_lr_sp_fishing_kit'],
            'field_liferaft_flag' => $data['field_liferaft_flag'],//
            'field_lr_sp_floating_knife' => $data['field_lr_sp_floating_knife'],
            'field_lr_floor_final_time_off' => $data['field_lr_floor_final_time_off'],
            'field_lr_floor_final_time_on' => $data['field_lr_floor_final_time_on'],
            'field_lr_floor_reading_off' => $data['field_lr_floor_reading_off'],
            'field_lr_floor_reading_on' => $data['field_lr_floor_reading_on'],
            'field_lr_floor_reeds_seams' => $data['field_lr_floor_reeds_seams'],
            'field_lr_floor_stretch_time_off' => $data['field_lr_floor_stretch_time_off'],
            'field_lr_floor_stretch_time_on' => $data['field_lr_floor_stretch_time_on'],
            'field_lr_floor_temp_off' => $data['field_lr_floor_temp_off'],
            'field_lr_floor_temp_on' => $data['field_lr_floor_temp_on'],
            'field_lr_fs_test' => $data['field_lr_fs_test'],
            'field_liferaft_agas_cylinder' => $gasCylinders,
            'field_lr_gas_inflation_test' => $data['field_lr_gas_inflation_test'],
            'field_lr_sp_heliograph' => $data['field_lr_sp_heliograph'],
            'field_lr_ie_high_pressure_hose' => $data['field_lr_ie_high_pressure_hose'],
            'field_lr_hru_expiry' => substr($data['field_lr_hru_expiry'], 0, 10),
            'field_lr_hru_maker' => $data['field_lr_hru_maker'],
            'field_lr_hru_sn' => $data['field_lr_hru_sn'],
            'field_lr_ie_infl_defl_valves' => $data['field_lr_ie_infl_defl_valves'],
            'field_lr_inner_canopy' => $data['field_lr_inner_canopy'],
            'field_lr_sp_inner_light' => $data['field_lr_sp_inner_light'],
            'field_lr_fs_latest_date' => substr($data['field_lr_fs_latest_date'], 0, 10),
            'field_lr_lt_latest_date' => substr($data['field_lr_lt_latest_date'], 0, 10),
            'field_lr_wp_latest_date' => substr($data['field_lr_wp_latest_date'], 0, 10),
            'field_lr_git_latest_date' => substr($data['field_lr_git_latest_date'], 0, 10),
            'field_lr_nap_latest_date' => substr($data['field_lr_nap_latest_date'], 0, 10),
            'field_lr_sp_leak_stoppers' => $data['field_lr_sp_leak_stoppers'],
            'field_liferaft_lifed_equipments' => $lifedEquip,
            'field_lr_lifelines' => $data['field_lr_lifelines'],
            'field_liferaft' => $liferaftID,
            'field_lr_liferafts_fabrics' => $data['field_lr_liferafts_fabrics'],
            'field_lr_load_test' => $data['field_lr_load_test'],
            'field_lr_lower_chamber' => $data['field_lr_lower_chamber'],
            'field_lr_lc_final_time_off' => $data['field_lr_lc_final_time_off'],
            'field_lr_lc_final_time_on' => $data['field_lr_lc_final_time_on'],
            'field_lr_lc_infl_defl_valve' => $data['field_lr_lc_infl_defl_valve'],
            'field_lr_lc_reading_off' => $data['field_lr_lc_reading_off'],
            'field_lr_lc_reading_on' => $data['field_lr_lc_reading_on'],
            'field_lr_lc_relief_valve' => $data['field_lr_lc_relief_valve'],
            'field_lr_lc_stretch_time_off' => $data['field_lr_lc_stretch_time_off'],
            'field_lr_lc_stretch_time_on' => $data['field_lr_lc_stretch_time_on'],
            'field_lr_lc_temp_off' => $data['field_lr_lc_temp_off'],
            'field_lr_lc_temp_on' => $data['field_lr_lc_temp_on'],
            'field_lr_container_marking_label' => $data['field_lr_container_marking_label'],
            'field_lr_mmd_inspection' => $data['field_lr_mmd_inspection'],
            'field_lr_nap_test' => $data['field_lr_nap_test'],
            'field_lr_ie_operating_head' => $data['field_lr_ie_operating_head'],
            'field_lr_outer_canopy' => $data['field_lr_outer_canopy'],
            'field_lr_outer_light' => $data['field_lr_outer_light'],
            'field_lr_sp_paddles' => $data['field_lr_sp_paddles'],
            'field_lr_ie_painter_cord' => $data['field_lr_ie_painter_cord'],
            'field_lr_painter_line_length_m_' => $data['field_lr_painter_line_length_m_'],
            'field_lr_sp_polythene_liner' => $data['field_lr_sp_polythene_liner'],
            'field_lr_sp_quoits_rescue_line' => $data['field_lr_sp_quoits_rescue_line'],
            'field_lr_sp_r_reflective_tape' => $data['field_lr_sp_r_reflective_tape'],
            'field_lr_sp_radar_reflector' => $data['field_lr_sp_radar_reflector'],
            'field_lr_rv_blow_off_lc' => $data['field_lr_rv_blow_off_lc'],
            'field_lr_rv_blow_off_uc' => $data['field_lr_rv_blow_off_uc'],
            'field_lr_rv_reset_lc' => $data['field_lr_rv_reset_lc'],
            'field_lr_rv_reset_uc' => $data['field_lr_rv_reset_uc'],
            'field_lr_ie_relief_valves' => $data['field_lr_ie_relief_valves'],
            'field_lr_sp_repair_kit' => $data['field_lr_sp_repair_kit'],
            'field_lr_ie_rubberized_bellow' => $data['field_lr_ie_rubberized_bellow'],
            'field_lr_sp_sea_anchor' => $data['field_lr_sp_sea_anchor'],
            'field_lr_container_seal_gasket' => $data['field_lr_container_seal_gasket'],
            'field_lr_container_sealing_strip' => $data['field_lr_container_sealing_strip'],
            'field_lr_sp_sickness_bags' => $data['field_lr_sp_sickness_bags'],
            'field_lr_sp_signaling_torch' => $data['field_lr_sp_signaling_torch'],
            'field_lr_sp_spare_bulb' => $data['field_lr_sp_spare_bulb'],
            'field_lr_sp_sponges' => $data['field_lr_sp_sponges'],
            'field_lr_stowage_height_m_' => $data['field_lr_stowage_height_m_'],
            'field_lr_strut_arches_seams' => $data['field_lr_strut_arches_seams'],
            'field_lr_sp_survival_book' => $data['field_lr_sp_survival_book'],
            'field_lr_sp_t_p_a_' => $data['field_lr_sp_t_p_a_'],
            'field_lr_ie_t_y_connectors' => $data['field_lr_ie_t_y_connectors'],
            'field_lr_sp_tin_openers' => $data['field_lr_sp_tin_openers'],
            'field_lr_sp_torch_cells' => $data['field_lr_sp_torch_cells'],
            'field_liferaft_type' => $data['field_liferaft_type'],
            'field_lr_upper_chamber' => $data['field_lr_upper_chamber'],
            'field_lr_uc_final_time_off' => $data['field_lr_uc_final_time_off'],
            'field_lr_uc_final_time_on' => $data['field_lr_uc_final_time_on'],
            'field_lr_uc_infl_defl_valve' => $data['field_lr_uc_infl_defl_valve'],
            'field_lr_uc_reading_off' => $data['field_lr_uc_reading_off'],
            'field_lr_uc_reading_on' => $data['field_lr_uc_reading_on'],
            'field_lr_uc_relief_valve' => $data['field_lr_uc_relief_valve'],
            'field_lr_uc_stretch_time_off' => $data['field_lr_uc_stretch_time_off'],
            'field_lr_uc_stretch_time_on' => $data['field_lr_uc_stretch_time_on'],
            'field_lr_uc_temp_off' => $data['field_lr_uc_temp_off'],
            'field_lr_uc_temp_on' => $data['field_lr_uc_temp_on'],
            'field_lr_sp_valise' => $data['field_lr_sp_valise'],
            'field_vessel' => $data['field_vessel'],
            'field_lr_water_pockets' => $data['field_lr_water_pockets'],
            'field_lr_weight' => $data['field_lr_weight'],
            'field_lr_sp_whistle' => $data['field_lr_sp_whistle'],
            'field_lr_wp_test' => $data['field_lr_wp_test'],
            'field_lr_zips_draw_cords' => $data['field_lr_zips_draw_cords'],
            'field_reinspection_comments' => $data['field_reinspection_comments'],
            'field_lr_drinking_vessel' => $data['field_lr_drinking_vessel'],
            'field_invoice' => $invocieID,
        ]);

        $job->save();

        $id = $job->id();
        $response = [$id];
        drupal_set_message("Created Job Successfully.");
        return new ResourceResponse($response);
    }

  }
}