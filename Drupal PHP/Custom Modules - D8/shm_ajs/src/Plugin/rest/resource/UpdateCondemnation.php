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
 * Class UpdateCondemnationJob
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for UpdateCondemnationJob entity.
 *
 * @RestResource(
 *   id="UpdateCondemnationJob_rest",
 *   label= @Translation("UpdateCondemnationJob Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/updatecondemnation_job"
 *   }
 * )
 */

class UpdateCondemnation extends ResourceBase{

    public function patch(array $data=[]){
        

        $cyl_tarid1 = $data['jobData']['gas_cylinders'];
        if($cyl_tarid1)
        {
            foreach ($cyl_tarid1 as $gascylinde) {
                $test =count($cyl_tarid1);
                if ($gascylinde['delete_flag'] == true)
                {
                    $gas_cylinderentity_id = $gascylinde['gas_cylinder_job_id'];
                    $gas_cylinder_node_info = JobSubmission::load($gas_cylinderentity_id);
                    $gas_cylinder_node_info->delete();
                    drupal_set_message("Deleted Gas Cylinder Job Successfully.");
                }elseif($gascylinde['gas_cylinder_job_id']) {

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
                            // 'field_gas_cylinde_refill_request' => $data['gas_cylinders'][$i]['refill_req'],
                            // 'field_gas_cylinder_ptest_request' => $data['gas_cylinders'][$i]['ptest_req'],
                        ]);
                        $gas_cylinder->save();
                        $gas_cylinder_job_id[] = $gas_cylinder->jid->value;
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


        if($data['entity_id'])
        {
            $jobInfo = JobSubmission::load($data['entity_id']);

            $jobInfo->field_condemnation_nap_test = $data['jobData']['field_nap_test'];
            $jobInfo->field_cr_acceptance_date =substr($data['jobData']['field_acceptance_date'],0,10);
            $jobInfo->field_cr_canopy_detoriation =$data['jobData']['field_canopy_detoriation'];
            $jobInfo->field_cr_condemned_after_vessel =$data['jobData']['field_condemned_after_vessel_casualty'];
            $jobInfo->field_cr_customer_acceptance =$data['jobData']['field_detailed_remark'];
            $jobInfo->field_cr_customer_request =$data['jobData']['field_customers_request'];
            $jobInfo->field_cr_damaged_by_mould =$data['jobData']['field_damaged_by_mould'];
            $jobInfo->field_cr_damaged_by_strong =$data['jobData']['field_damaged_by_strong_heat'];
            $jobInfo->field_cr_damaged_during_demon=$data['jobData']['field_damaged_during_demonstration'];
            $jobInfo->field_cr_detailed_remark =$data['jobData']['field_customer_acceptance'];
            $jobInfo->field_cr_gas_inflation_gs_test =$data['jobData']['field_gas_inflation_test'];
            $jobInfo->field_cr_glue_delamination =$data['jobData']['field_glue_delamination'];
            $jobInfo->field_cr_leakage_beyond_econo =$data['jobData']['field_leakage_beyond_economiacl_repair'];
            $jobInfo->field_cr_oxidation_copper =$data['jobData']['field_oxidation'];
            $jobInfo->field_cr_seperation_of_adhesive =$data['jobData']['field_seperation_of_adhesive'];
            $jobInfo->field_cr_vandalism =$data['jobData']['field_vandalism'];
            $jobInfo->field_cr_water_damaged =$data['jobData']['field_water_damaged'];
            $jobInfo->field_cr_wear_chafing =$data['jobData']['field_wear_chafing'];
            $jobInfo->field_cylinders_details = $gas_cylinder_job_id;
			$jobInfo->field_service_engineer = $data['jobData']['field_service_engineer']['uid'];
            $jobInfo->moderation_state = $moderation_state;
                $jobInfo->save();
        }


		drupal_set_message("Updated Job Successfully.");

        $response = ['message' => 'Get liferaft test'];
        return new ResourceResponse($response);
    }
}