<?php

namespace Drupal\shm_ajs\Plugin\rest\resource;
use Drupal\node\Entity\Node;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\shm_services_entity\Entity\JobSubmission;

/**
 *
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for Condemnation entity.
 *
 * @RestResource(
 *   id="condemnation_job_rest",
 *   label= @Translation("Condemnation Job Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/condemnation-job",
 *    "https://www.drupal.org/link-relations/create" = "/rest_api/condemnation-job"
 *   }
 * )
 */

class CondemnationtJob extends ResourceBase
{
    public function get()
    {

    }
    public function post(array $data=[])
    {
        if ($data['liferaft']['isNew'])
        {
            // Code to create new Liferaft entity for submitted job.
            $node_liferaft = Node::create([
                'type' => 'liferafts',
                'title' => $data['liferaft']['field_liferaft_SN'],
                'field_capacity' => $data['liferaft']['field_liferaft_capacity'],
                'field_last_surveyed_on' => substr($data['liferaft']['field_liferaft_date_inspection'],0,10),
                'field_manufacturer' => $data['liferaftNew']['field_liferaft_manufacturer']['nid'],
                'field_manufacturing_date' => substr($data['liferaft']['field_liferaft_date_manufacturing'],0,10),
                'field_liferaft_model' => $data['liferaft']['field_liferaft_model'],
                'field_operating_mechanism' => $data['liferaft']['field_liferaft_operating_mechanism'],
                'field_lr_solas_pack_type' => $data['liferaft']['field_liferaft_survivalpack_type'],
                'field_surveyed_on' => substr($data['liferaft']['field_liferaft_date_inspection'],0,10),
                'field_type_capacity' => $data['liferaftNew']['field_liferaft_type']['tid'],
                'field_vessel' => $data['liferaft']['vessel']['nid'],
				'field_codemned_liferaft' => 1,
            ]);

            $node_liferaft->save();
            $liferaftID = $node_liferaft->nid->value;
        }
        else {
            // Liferaft already exists
             $liferaftID = $data['liferaft']['liferaft_id'];
            $liferaft = Node::load($liferaftID);
            $liferaft->set('field_last_surveyed_on', substr($data['liferaft']['field_liferaft_previous_inspec'],0,10));
            $liferaft->set('field_surveyed_on',substr($data['liferaft']['field_liferaft_date_inspection'],0,10));
			$liferaft->set('field_codemned_liferaft', 1);
            $liferaft->save();
        }
        $gas_cylinder_count = count($data['gas_cylinders']);
        for($i=0;$i<$gas_cylinder_count;$i++)
        {
            $gas_cylinder = JobSubmission::create([
                'type' => 'liferaft_gas_cylinder_details',
                'field_liferaft_cylinder_sn' => $data['gas_cylinders'][$i]['cylinder_sn'],
                'field_liferaft_full_weight_kgs' => $data['gas_cylinders'][$i]['full_weight'],
                'field_liferaft_tare_weight_kgs' => $data['gas_cylinders'][$i]['tare_weight'],
                'field_liferaft_gas_weight_n2_kgs' => $data['gas_cylinders'][$i]['gas_weight_n2'],
                'field_liferaft_gas_weight_co2_kg' => $data['gas_cylinders'][$i]['gas_weight_co2'],
                'field_liferaft_actual_full_weigt' => $data['gas_cylinders'][$i]['actual_full_weight'],
                'field_gas_cylinder_lpt_date' => substr($data['gas_cylinders'][$i]['lpt'],0,10),
               // 'field_gas_cylinde_refill_request' => $data['gas_cylinders'][$i]['refill_req'],
               // 'field_gas_cylinder_ptest_request' => $data['gas_cylinders'][$i]['ptest_req'],
            ]);
            $gas_cylinder->save();
            $gas_cylinder_job_id[] = $gas_cylinder->jid->value;

        }

        if($data['field_detailed_remark'] == 'yes')
        {
            $detailed_remark = 1;
        }else{
            $detailed_remark = 0;
        }

        if($data['modeartion_state'] == 'draft')
        {
            $moderation_state = 'draft';
        }elseif ($data['modeartion_state'] == 'pending_approval')
        {
            $moderation_state = 'approved';
        }elseif ($data['modeartion_state'] == 'approved')
        {
            $moderation_state = 'approved';
        }
        if($data['liferaft']['vessel']){
       $vessel_id= $data['liferaft']['vessel']['nid'];
        }
        if($data['field_service_engineer']){
            $serviceeng_uid = $data['field_service_engineer']['uid'];
        }

        $condemnation_job = JobSubmission::create([
            'type' => 'condemnation_report',
            'moderation_state' => $moderation_state,
            'field_cylinders_details' => $gas_cylinder_job_id,
			'field_service_engineer' => $serviceeng_uid,
            'field_liferaft_details' => $liferaftID,
            'field_vessel_details' => $vessel_id,
            'field_cr_acceptance_date' => substr($data['field_acceptance_date'],0,10),
            'field_condemnation_nap_test' => $data['field_nap_test'],
            'field_condem_floor_seam_fs_test' => $data['field_condem_floor_seam_fs_test'],
            'field_cr_gas_inflation_gs_test' => $data['field_gas_inflation_test'],
            'field_cr_leakage_beyond_econo' => $data['field_leakage_beyond_economiacl_repair'],
            'field_cr_seperation_of_adhesive' => $data['field_seperation_of_adhesive'],
            'field_cr_damaged_by_mould' => $data['field_damaged_by_mould'],
            'field_cr_water_damaged' => $data['field_water_damaged'],
            'field_cr_damaged_during_demon' => $data['field_damaged_during_demonstration'],
            'field_cr_canopy_detoriation' => $data['field_canopy_detoriation'],
            'field_cr_damaged_by_strong' => $data['field_damaged_by_strong_heat'],
            'field_cr_wear_chafing' => $data['field_wear_chafing'],
            'field_cr_condemned_after_vessel' => $data['field_condemned_after_vessel_casualty'],
            'field_cr_oxidation_copper' => $data['field_oxidation'],
            'field_cr_vandalism' => $data['field_vandalism'],
            'field_cr_customer_request' => $data['field_customers_request'],
            'field_cr_glue_delamination' => $data['field_glue_delamination'],
            'field_cr_customer_acceptance' => $detailed_remark,
            'field_cr_detailed_remark' =>$data['field_customer_acceptance']
            ]);


        $condemnation_job->save();
        $condemnation_job_id = $condemnation_job->id();
        $response = [$condemnation_job_id];
		drupal_set_message("Created Job Successfully.");
        return new ResourceResponse($response);
    //    drupal_set_message("test");
    }
}