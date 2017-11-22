<?php
/**
 * Created by PhpStorm.
 * User: Piyusha Pokharana
 * Date: 10/25/2017
 * Time: 1:35 PM
 */

namespace Drupal\shm_ajs\Plugin\rest\resource;

use Drupal\Core\Database\Database;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Symfony\Component\DependencyInjection\ContainerInterface;
/**
 * Class EditCondemnationJob
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for EditCondemnationJob entity.
 *
 * @RestResource(
 *   id="editCondemnationJob_rest",
 *   label= @Translation("EditCondemnationJob Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/editCondemnation/{entity}"
 *   }
 * )
 */

class EditCondemnationJob extends ResourceBase{

    public function get($entity = null)
    {

        if ($entity) {
            $jobInfo = \Drupal\shm_services_entity\Entity\JobSubmission::load($entity);
            $lr_nodeinfo = $jobInfo->get('field_liferaft_details')->first()->get('entity')->getTarget();
              if($jobInfo->get('field_liferaft_details')) {
                $lr_nodeinfo = $jobInfo->get('field_liferaft_details')->first()->get('entity')->getTarget();
                $liferaft_SN = $lr_nodeinfo->get('title')->getString();
                if ($lr_nodeinfo->get('field_type_capacity')->first()) {

                $liferaft_type_capacity = $lr_nodeinfo->get('field_type_capacity')->first()->get('entity')->getTarget()->get('name')->getString();
                $liferaft_field_capacity = $lr_nodeinfo->get('field_type_capacity')->first()->get('entity')->getTarget()->get('field_capacity')->getString();

            }
                $liferaft_field_last_surveyed_on = $lr_nodeinfo->get('field_last_surveyed_on')->getString();
                $liferaft_field_liferaft_model = $lr_nodeinfo->get('field_liferaft_model')->getString();
                $liferaft_field_lr_solas_pack_type = $lr_nodeinfo->get('field_lr_solas_pack_type')->getString();
                if ($lr_nodeinfo->get('field_manufacturer')->first())
                {
                    $liferaft_field_manufacturer = $lr_nodeinfo->get('field_manufacturer')->first()->get('entity')->getTarget()->get('title')->getString();

                }
                $liferaft_field_manufacturing_date = $lr_nodeinfo->get('field_manufacturing_date')->getString();
                $liferaft_field_surveyed_on = $lr_nodeinfo->get('field_surveyed_on')->getString();
            }


            $vessel = $lr_nodeinfo->get('field_vessel')->first()->get('entity')->getTarget();
            $vesselArray = array();
            if($vessel)
            {
                $vesselArray = array(
                    'title' => $vessel->get('title')->getString(),
                    'field_liferaft_vessel_imo_no' => $vessel->get('field_vessel_imo_no')->getString(),
                    'field_liferaft_vesel_owner_agent' => $vessel->get('field_company')->first()->get('entity')->getTarget()->get('title')->getString(),
                    'field_liferaft_vessel_call_sign' => $vessel->get('field_vessel_call')->getString(),
                    'field_liferaft_flag' => $vessel->get('field_vessel_flag')->getString(),
                );
            }

            $cyl_tarid1 = $jobInfo->get('field_cylinders_details');
            $gasCylinders = array();
            foreach ($cyl_tarid1 as $entity) {
                $cyl_id = $entity->get('target_id')->getString();
                if($cyl_id)
                {
                    $cyl_info = \Drupal\shm_services_entity\Entity\JobSubmission::load($cyl_id);
                    $gasCylinders[] = array(
						'gas_cylinder_job_id' =>$cyl_id,
                        'actual_full_weight' => $cyl_info->get('field_liferaft_actual_full_weigt')->getString(),
                        'cylinder_sn' => $cyl_info->get('field_liferaft_cylinder_sn')->getString(),
                        'tare_weight' => $cyl_info->get('field_liferaft_tare_weight_kgs')->getString(),
                        'gas_weight_co2' => $cyl_info->get('field_liferaft_gas_weight_co2_kg')->getString(),
                        'gas_weight_n2' => $cyl_info->get('field_liferaft_gas_weight_n2_kgs')->getString(),
                        'full_weight' => $cyl_info->get('field_liferaft_full_weight_kgs')->getString(),
                        'lpt' => $cyl_info->get('field_gas_cylinder_lpt_date')->getString(),
                       // 'ptest_req' => $cyl_info->get('field_gas_cylinder_ptest_request')->getString(),
                       // 'refill_req' => $cyl_info->get('field_gas_cylinde_refill_request')->getString(),
                    );
                }

            }
			 if($jobInfo->get('field_service_engineer')->first())
            {
                $first_name = $jobInfo->get('field_service_engineer')->first()->get('entity')->getTarget()->get('field_first_name')->getString();
                $last_name = $jobInfo->get('field_service_engineer')->first()->get('entity')->getTarget()->get('field_last_name')->getString();
                $uid= $jobInfo->get('field_service_engineer')->getString();
                $serviceEngg = array('name' => $first_name. ' '.$last_name,
                    'uid' => $uid);
            }
			
                $response = array(
                    'field_nap_test' => $jobInfo->get('field_condemnation_nap_test')->getString(),
                    'field_floor_seam_test' => $jobInfo->get('field_condem_floor_seam_fs_test')->getString(),
                    'field_acceptance_date' => $jobInfo->get('field_cr_acceptance_date')->getString(),
                    'field_canopy_detoriation' => $jobInfo->get('field_cr_canopy_detoriation')->getString(),
                    'field_condemned_after_vessel_casualty' => $jobInfo->get('field_cr_condemned_after_vessel')->getString(),
                    'field_detailed_remark' => $jobInfo->get('field_cr_customer_acceptance')->getString(),
                    'field_customers_request' => $jobInfo->get('field_cr_customer_request')->getString(),
                    'field_damaged_by_mould' => $jobInfo->get('field_cr_damaged_by_mould')->getString(),
                    'field_damaged_by_strong_heat' => $jobInfo->get('field_cr_damaged_by_strong')->getString(),
                    'field_damaged_during_demonstration' => $jobInfo->get('field_cr_damaged_during_demon')->getString(),
                    'field_customer_acceptance' => $jobInfo->get('field_cr_detailed_remark')->getString(),
                    'field_gas_inflation_test' => $jobInfo->get('field_cr_gas_inflation_gs_test')->getString(),
                    'field_glue_delamination' => $jobInfo->get('field_cr_glue_delamination')->getString(),
                    'field_leakage_beyond_economiacl_repair' => $jobInfo->get('field_cr_leakage_beyond_econo')->getString(),
                    'field_oxidation' => $jobInfo->get('field_cr_oxidation_copper')->getString(),
                    'field_seperation_of_adhesive' => $jobInfo->get('field_cr_seperation_of_adhesive')->getString(),
                    'field_vandalism' => $jobInfo->get('field_cr_vandalism')->getString(),
                    'field_water_damaged' => $jobInfo->get('field_cr_water_damaged')->getString(),
                    'field_wear_chafing' => $jobInfo->get('field_cr_wear_chafing')->getString(),
                    'moderation_state' =>    $jobInfo->get('moderation_state')->getString(),
                    'liferaft' => array(
                        'field_liferaft_SN' => $liferaft_SN,
                        'field_liferaft_serial_no' => $liferaft_SN,
                         'field_liferaft_type' => array(
                            'type' => $liferaft_type_capacity,
                            'capacity' => $liferaft_field_capacity
                        ),
                        'field_liferaft_previous_inspec' => $liferaft_field_last_surveyed_on,
                        'field_liferaft_model' => $liferaft_field_liferaft_model,
                        'field_liferaft_survivalpack_type' => $liferaft_field_lr_solas_pack_type,
                        'field_liferaft_manufacturer' => array('title' => $liferaft_field_manufacturer),
                        'field_liferaft_date_manufacturing' => $liferaft_field_manufacturing_date,
                        'field_liferaft_date_inspection' => $liferaft_field_surveyed_on,

                        'vessel' => $vesselArray,
                    ),
                    'gas_cylinders' => $gasCylinders,
					'field_service_engineer' => $serviceEngg,

                );

            return new ResourceResponse($response);
        }
    }
}