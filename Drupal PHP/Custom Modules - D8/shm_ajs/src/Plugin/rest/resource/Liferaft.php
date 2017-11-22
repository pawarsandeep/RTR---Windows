<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 9/26/2017
 * Time: 2:01 AM
 */

namespace Drupal\shm_ajs\Plugin\rest\resource;

use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Session\AccountProxyInterface;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\node\Entity;
use Drupal\shm_services_entity\Entity\JobSubmission;
use Psr\Log\LoggerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Database\Database;

/**
 * Class Liferaft
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for Liferaft entity.
 *
 * @RestResource(
 *   id="liferaft_rest",
 *   label= @Translation("Liferaft Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/liferaft/{entity}",
 *    "https://www.drupal.org/link-relations/create" = "/rest_api/liferaft/get_by_title"
 *   }
 * )
 */
class Liferaft extends ResourceBase
{
    protected $currentUser;
    protected $entityManager;

    public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
        return new static(
            $configuration,
            $plugin_id,
            $plugin_definition,
            $container->getParameter('serializer.formats'),
            $container->get('logger.factory')->get('rest'),
            $container->get('entity.manager'),
            $container->get('current_user')
        );
    }

    public function __construct(array $configuration, $plugin_id, $plugin_definition, array $serializer_formats, \Psr\Log\LoggerInterface $logger, EntityManagerInterface $entity_manager,AccountProxyInterface $current_user) {
        parent::__construct($configuration, $plugin_id, $plugin_definition, $serializer_formats, $logger);
        $this->currentUser = $current_user;
        $this->entityManager = $entity_manager;
    }

    public function get($entity=NULL){
        if($entity){


            $liferaft = Entity\Node::load($entity);
            $vessel_arr = "";
            if($liferaft->get('field_vessel')->first()) {
                $vessel = $liferaft->get('field_vessel')->first()->get('entity')->getTarget();
                $vessel_arr[] =  array(
                    'nid' => $vessel->get('nid')->getString(),
                    'title' => $vessel->get('title')->getString(),
                    'field_liferaft_vessel_imo_no'=>$vessel->get('field_vessel_imo_no')->getString(),
                    'field_liferaft_vesel_owner_agent'=>$vessel->get('field_company')->first()->get('entity')->getTarget()->get('title')->getString(),
                    'field_liferaft_vessel_call_sign'=>$vessel->get('field_vessel_call')->getString(),
                    'field_liferaft_flag'=>$vessel->get('field_vessel_flag')->getString(),
                );
            }
            $connection = Database::getConnection();

            $query = $connection->select('job_entity__field_liferaft', 'field_liferaft');
            $query->join('job_submission_field_data','job_data','job_data.jid=field_liferaft.entity_id');
            $query->fields('field_liferaft',array('entity_id'));
            $query->condition('field_liferaft.bundle', 'liferaft_service_job', '=');
            $query->condition('field_liferaft.field_liferaft_target_id',"$entity", '=');
            $query->condition('job_data.type','liferaft_service_job', '=');
            $query->orderBy('job_data.created', 'DESC');
            $query->range(0,1);
            $data = $query->execute();
            $latest_job = $data->fetchAll(\PDO::FETCH_ASSOC);
            $liferaft_type_capacity=null;
            $liferaft_field_capacity=null;
            if ($liferaft->get('field_type_capacity')->first()) {

                $liferaft_type_capacity = $liferaft->get('field_type_capacity')->first()->get('entity')->getTarget()->get('name')->getString();
                $liferaft_field_capacity = $liferaft->get('field_type_capacity')->first()->get('entity')->getTarget()->get('field_capacity')->getString();

            }
            $date_of_prev_inspection = $liferaft->get('field_surveyed_on')->getString();
            if($date_of_prev_inspection)
            {
                $date_of_prev_inspection1 = $liferaft->get('field_surveyed_on')->getString();
            }
            else
            {
                $date_of_prev_inspection1 = date('Y-m-d');
            }
            if($latest_job) {
                $reinspection_job_info = JobSubmission::load($latest_job[0]['entity_id']);

				$gasCylinders = array();
                $lifed_ele = array();
                $job_certificate = $reinspection_job_info->get('field_liferaft_certificate_no')->getString();
                $gas_cylinder_id = $reinspection_job_info->get('field_liferaft_agas_cylinder');
                foreach ($gas_cylinder_id as $entity) {
                    $cyl_id = $entity->get('target_id')->getString();
                    if ($cyl_id) {
                        $cyl_info = \Drupal\shm_services_entity\Entity\JobSubmission::load($cyl_id);
                        $gasCylinders[] = array(
                            'gas_cylinder_job_id' => $cyl_id,
                            'actual_full_weight' => $cyl_info->get('field_liferaft_actual_full_weigt')->getString(),
                            'cylinder_sn' => $cyl_info->get('field_liferaft_cylinder_sn')->getString(),
                            'tare_weight' => $cyl_info->get('field_liferaft_tare_weight_kgs')->getString(),
                            'gas_weight_co2' => $cyl_info->get('field_liferaft_gas_weight_co2_kg')->getString(),
                            'gas_weight_n2' => $cyl_info->get('field_liferaft_gas_weight_n2_kgs')->getString(),
                            'full_weight' => $cyl_info->get('field_liferaft_full_weight_kgs')->getString(),
                            'lpt' => $cyl_info->get('field_gas_cylinder_lpt_date')->getString(),
                            //  'ptest_req' => $cyl_info->get('field_gas_cylinder_ptest_request')->getString(),
                            //  'refill_req' => $cyl_info->get('field_gas_cylinde_refill_request')->getString(),
                        );
                    }
                }



                $lifed_equip_id = $reinspection_job_info->get('field_liferaft_lifed_equipments');
                foreach ($lifed_equip_id as $entity) {
                    $lifed_id = $entity->get('target_id')->getString();
                    if ($lifed_id) {
                        $lifed_info = \Drupal\shm_services_entity\Entity\JobSubmission::load($lifed_id);
                        $lifed_ele[] = array(
                            'field_liferaft_expiry' => $lifed_info->get('field_liferaft_expiry')->getString(),
                            'field_liferaft_lifed_items' => $lifed_info->get('field_liferaft_lifed_items')->getString(),
                            'field_liferaft_lot_no' => $lifed_info->get('field_liferaft_lot_no')->getString(),
                            'field_liferaft_quantity' => $lifed_info->get('field_liferaft_quantity')->getString(),
                            'field_liferaft_replaced_expiry' => $lifed_info->get('field_liferaft_replaced_expiry')->getString(),
                            'field_liferaft_replaced_lot_no' => $lifed_info->get('field_liferaft_replaced_lot_no')->getString(),
                            'field_liferaft_replaced_quantity' => $lifed_info->get('field_liferaft_replaced_quantity')->getString(),
                            'field_lr_manufacturer' => $lifed_info->get('field_lr_manufacturer')->getString(),
                            'field_lr_replaced_manufacturer' => $lifed_info->get('field_lr_replaced_manufacturer')->getString(),
                        );
                    }
                }
                if($reinspection_job_info->get('field_service_engineer')->first()) {
                    $first_name = $reinspection_job_info->get('field_service_engineer')->first()->get('entity')->getTarget()->get('field_first_name')->getString();
                    $last_name = $reinspection_job_info->get('field_service_engineer')->first()->get('entity')->getTarget()->get('field_last_name')->getString();
                }
                if($liferaft->get('field_manufacturer')->first()){
                   $liferaft_manu = $liferaft->get('field_manufacturer')->first()->get('entity')->getTarget()->get('title')->getString();
                }
                $response = array(
                    'liferaft' => array(
                        'field_liferaft_date_inspection'=> date('Y-m-d'),
                        'field_liferaft_previous_inspec'=>$date_of_prev_inspection1,
                        'field_liferaft_manufacturer'=>array('title'=> $liferaft_manu),
                        'field_liferaft_serial_no'=>$liferaft->getTitle(),
                        'field_liferaft_certificate_no' => $job_certificate,
                        'field_liferaft_model'=>$liferaft->get('field_liferaft_model')->getString(),
                         'field_liferaft_type' => array(
                            'type' => $liferaft_type_capacity,
                            'capacity' => $liferaft_field_capacity
                        ),
                        'field_liferaft_date_manufacturing'=>$liferaft->get('field_manufacturing_date')->getString(),
                        'field_liferaft_survivalpack_type'=>$liferaft->get('field_lr_solas_pack_type')->getString(),
                        'liferaft_id' => $liferaft->id(),
                        'vessel'=>$vessel_arr,
                    ),
                   'field_service_engineer' => array(
                        'name' => $first_name.$last_name,
                    ),

                    'gas_cylinders' => $gasCylinders,
                    /*----------------------lifed equipment-------------------*/
                    'lifed_equipments' => $lifed_ele,

                    /*---------------------Pressure Test----------------------*/
                    'field_lr_uc_stretch_time_off' => $reinspection_job_info->get('field_lr_uc_stretch_time_off')->getString(),
                    'field_lr_uc_stretch_time_on' => $reinspection_job_info->get('field_lr_uc_stretch_time_on')->getString(),
                    'field_lr_uc_final_time_off' =>$reinspection_job_info->get('field_lr_uc_final_time_off')->getString(),
                    'field_lr_uc_final_time_on' =>$reinspection_job_info->get('field_lr_uc_final_time_on')->getString(),
                    'field_lr_uc_reading_off' =>$reinspection_job_info->get('field_lr_uc_reading_off')->getString(),
                    'field_lr_uc_reading_on' =>$reinspection_job_info->get('field_lr_uc_reading_on')->getString(),
                    'field_lr_uc_stretch_time_on' => $reinspection_job_info->get('field_lr_uc_stretch_time_on')->getString(),
                    'field_lr_uc_temp_off' =>$reinspection_job_info->get('field_lr_uc_temp_off')->getString(),
                    'field_lr_rv_blow_off_uc' =>$reinspection_job_info->get('field_lr_rv_blow_off_uc')->getString(),
                    'field_lr_rv_reset_lc' =>$reinspection_job_info->get('field_lr_rv_reset_lc')->getString(),
                    'field_lr_rv_blow_off_lc' =>$reinspection_job_info->get('field_lr_rv_blow_off_lc')->getString(),
                    'field_lr_rv_reset_uc' =>$reinspection_job_info->get('field_lr_rv_reset_uc')->getString(),

                    'field_lr_lc_final_time_off' =>$reinspection_job_info->get('field_lr_lc_final_time_off')->getString(),
                    'field_lr_lc_final_time_on' =>$reinspection_job_info->get('field_lr_lc_final_time_on')->getString(),
                    'field_lr_lc_reading_off' => $reinspection_job_info->get('field_lr_lc_reading_off')->getString(),
                    'field_lr_lc_reading_on' => $reinspection_job_info->get('field_lr_lc_reading_on')->getString(),
                    'field_lr_lc_stretch_time_off' => $reinspection_job_info->get('field_lr_lc_stretch_time_off')->getString(),
                    'field_lr_lc_stretch_time_on' => $reinspection_job_info->get('field_lr_lc_stretch_time_on')->getString(),
                    'field_lr_lc_temp_off' => $reinspection_job_info->get('field_lr_lc_temp_off')->getString(),
                    'field_lr_lc_temp_on' => $reinspection_job_info->get('field_lr_lc_temp_on')->getString(),

                    'field_lr_floor_final_time_off' => $reinspection_job_info->get('field_lr_floor_final_time_off')->getString(),
                    'field_lr_floor_final_time_on' => $reinspection_job_info->get('field_lr_floor_final_time_on')->getString(),
                    'field_lr_floor_stretch_time_off' => $reinspection_job_info->get('field_lr_floor_stretch_time_off')->getString(),
                    'field_lr_floor_stretch_time_on' =>  $reinspection_job_info->get('field_lr_floor_stretch_time_on')->getString(),
                    'field_lr_floor_temp_off' =>$reinspection_job_info->get('field_lr_floor_temp_off')->getString(),
                    'field_lr_floor_temp_on' => $reinspection_job_info->get('field_lr_floor_temp_on')->getString(),
                    'field_lr_floor_reading_off' => $reinspection_job_info->get('field_lr_floor_reading_off')->getString(),
                    'field_lr_floor_reading_on' => $reinspection_job_info->get('field_lr_floor_reading_on')->getString(),
                    /*---------------------Containers----------------------*/
                    'field_lr_container_seal_gasket' =>$reinspection_job_info->get('field_lr_container_seal_gasket')->getString(),
                    'field_lr_container_sealing_strip'=>$reinspection_job_info->get('field_lr_container_sealing_strip')->getString(),
                    'field_lr_container_marking_label'=>$reinspection_job_info->get('field_lr_container_marking_label')->getString(),
                    'field_lr_f_r_p_container'=>$reinspection_job_info->get('field_lr_f_r_p_container')->getString(),
                    'field_lr_container_container_id'=>$reinspection_job_info->get('field_lr_container_container_id')->getString(),
                    'field_lr_container_breaking_band'=>$reinspection_job_info->get('field_lr_container_breaking_band')->getString(),
                    /*---------------------INFLATION/DEFLATION ITEMS----------------------*/
                    'field_lr_ie_operating_head' =>$reinspection_job_info->get('field_lr_ie_operating_head')->getString(),
                    'field_lr_ie_high_pressure_hose'=>$reinspection_job_info->get('field_lr_ie_high_pressure_hose')->getString(),
                    'field_lr_ie_t_y_connectors'=>$reinspection_job_info->get('field_lr_ie_t_y_connectors')->getString(),
                    'field_lr_ie_infl_defl_valves'=>$reinspection_job_info->get('field_lr_ie_infl_defl_valves')->getString(),
                    'field_lr_ie_rubberized_bellow'=>$reinspection_job_info->get('field_lr_ie_rubberized_bellow')->getString(),
                    'field_lr_ie_painter_cord'=>$reinspection_job_info->get('field_lr_ie_painter_cord')->getString(),
                    'field_lr_ie_relief_valves' =>$reinspection_job_info->get('field_lr_ie_relief_valves')->getString(),
                    'field_lr_lc_infl_defl_valve'=>$reinspection_job_info->get('field_lr_lc_infl_defl_valve')->getString(),
                    'field_lr_lc_relief_valve'=>$reinspection_job_info->get('field_lr_lc_relief_valve')->getString(),
                    'field_lr_uc_infl_defl_valve'=>$reinspection_job_info->get('field_lr_f_r_p_container')->getString(),
                    'field_lr_uc_relief_valve'=>$reinspection_job_info->get('field_lr_uc_relief_valve')->getString(),
                    /*---------------------Liferaft----------------------*/

                    'field_lr_boarding_ladder' =>$reinspection_job_info->get('field_lr_boarding_ladder')->getString(),
                    'field_lr_boarding_ramp'=>$reinspection_job_info->get('field_lr_boarding_ramp')->getString(),
                    'field_lr_lifelines'=>$reinspection_job_info->get('field_lr_lifelines')->getString(),
                    'field_lr_liferafts_fabrics'=>$reinspection_job_info->get('field_lr_liferafts_fabrics')->getString(),
                    'field_lr_water_pockets'=>$reinspection_job_info->get('field_lr_water_pockets')->getString(),
                    'field_lr_zips_draw_cords'=>$reinspection_job_info->get('field_lr_zips_draw_cords')->getString(),
                    'field_lr_lower_chamber' =>$reinspection_job_info->get('field_lr_lower_chamber')->getString(),
                    'field_lr_upper_chamber'=>$reinspection_job_info->get('field_lr_upper_chamber')->getString(),
                    'field_lr_floor_reeds_seams'=>$reinspection_job_info->get('field_lr_floor_reeds_seams')->getString(),
                    'field_lr_strut_arches_seams'=>$reinspection_job_info->get('field_lr_strut_arches_seams')->getString(),
                    'field_lr_outer_light'=>$reinspection_job_info->get('field_lr_outer_light')->getString(),
                    'field_lr_inner_canopy'=>$reinspection_job_info->get('field_lr_inner_canopy')->getString(),
                    'field_lr_outer_canopy'=>$reinspection_job_info->get('field_lr_outer_canopy')->getString(),
                    'field_lr_sp_r_reflective_tape'=>$reinspection_job_info->get('field_lr_sp_r_reflective_tape')->getString(),
                    /*--------------------SURVIVAL PACK---------------------------------*/
                    'field_lr_sp_adhesive_tube' =>$reinspection_job_info->get('field_lr_sp_adhesive_tube')->getString(),
                    'field_lr_sp_bailers'=>$reinspection_job_info->get('field_lr_sp_bailers')->getString(),
                    'field_lr_sp_bellow'=>$reinspection_job_info->get('field_lr_sp_bellow')->getString(),
                    'field_lr_sp_fishing_kit'=>$reinspection_job_info->get('field_lr_sp_fishing_kit')->getString(),
                    'field_lr_sp_floating_knife'=>$reinspection_job_info->get('field_lr_sp_floating_knife')->getString(),
                    'field_lr_sp_heliograph'=>$reinspection_job_info->get('field_lr_sp_heliograph')->getString(),
                    'field_lr_sp_inner_light' =>$reinspection_job_info->get('field_lr_sp_inner_light')->getString(),
                    'field_lr_sp_leak_stoppers'=>$reinspection_job_info->get('field_lr_sp_leak_stoppers')->getString(),
                    'field_lr_sp_paddles'=>$reinspection_job_info->get('field_lr_sp_paddles')->getString(),
                    'field_lr_sp_polythene_liner'=>$reinspection_job_info->get('field_lr_sp_polythene_liner')->getString(),

                    'field_lr_sp_quoits_rescue_line' =>$reinspection_job_info->get('field_lr_sp_quoits_rescue_line')->getString(),
                    'field_lr_sp_r_reflective_tape'=>$reinspection_job_info->get('field_lr_sp_r_reflective_tape')->getString(),
                    'field_lr_sp_radar_reflector'=>$reinspection_job_info->get('field_lr_sp_radar_reflector')->getString(),
                    'field_lr_sp_repair_kit'=>$reinspection_job_info->get('field_lr_sp_repair_kit')->getString(),
                    'field_lr_sp_sea_anchor'=>$reinspection_job_info->get('field_lr_sp_sea_anchor')->getString(),
                    'field_lr_sp_sickness_bags'=>$reinspection_job_info->get('field_lr_sp_sickness_bags')->getString(),
                    'field_lr_sp_signaling_torch' =>$reinspection_job_info->get('field_lr_sp_signaling_torch')->getString(),
                    'field_lr_sp_spare_bulb'=>$reinspection_job_info->get('field_lr_sp_spare_bulb')->getString(),
                    'field_lr_sp_sponges'=>$reinspection_job_info->get('field_lr_sp_sponges')->getString(),
                    'field_lr_sp_survival_book'=>$reinspection_job_info->get('field_lr_sp_survival_book')->getString(),

                    'field_lr_sp_t_p_a_'=>$reinspection_job_info->get('field_lr_sp_t_p_a_')->getString(),
                    'field_lr_sp_tin_openers'=>$reinspection_job_info->get('field_lr_sp_tin_openers')->getString(),
                    'field_lr_sp_torch_cells'=>$reinspection_job_info->get('field_lr_sp_torch_cells')->getString(),
                    'field_lr_sp_valise'=>$reinspection_job_info->get('field_lr_sp_valise')->getString(),
                    'field_lr_sp_whistle'=>$reinspection_job_info->get('field_lr_sp_whistle')->getString(),
                    'field_lr_drinking_vessel'=>$reinspection_job_info->get('field_lr_drinking_vessel')->getString(),


                    /*-----------------Other-----------------*/
                    'field_lr_hru_expiry'=>$reinspection_job_info->get('field_lr_hru_expiry')->getString(),
                    'field_lr_hru_sn'=>$reinspection_job_info->get('field_lr_hru_sn')->getString(),
                    'field_lr_hru_maker'=>$reinspection_job_info->get('field_lr_hru_maker')->getString(),
                    'field_lr_wp_test'=>$reinspection_job_info->get('field_lr_wp_test')->getString(),
                    'field_lr_wp_latest_date'=>$reinspection_job_info->get('field_lr_wp_latest_date')->getString(),
                    'field_lr_nap_test'=>$reinspection_job_info->get('field_lr_nap_test')->getString(),
                    'field_lr_nap_latest_date'=>$reinspection_job_info->get('field_lr_nap_latest_date')->getString(),
                    'field_lr_fs_test'=>$reinspection_job_info->get('field_lr_fs_test')->getString(),
                    'field_lr_fs_latest_date'=>$reinspection_job_info->get('field_lr_fs_latest_date')->getString(),
                    'field_lr_gas_inflation_test'=>$reinspection_job_info->get('field_lr_gas_inflation_test')->getString(),
                    'field_lr_git_latest_date'=>$reinspection_job_info->get('field_lr_git_latest_date')->getString(),
                    'field_lr_load_test'=>$reinspection_job_info->get('field_lr_load_test')->getString(),
                    'field_lr_lt_latest_date'=>$reinspection_job_info->get('field_lr_lt_latest_date')->getString(),
                    'field_lr_weight'=>$reinspection_job_info->get('field_lr_weight')->getString(),
                    'field_lr_painter_line_length_m_'=>$reinspection_job_info->get('field_lr_painter_line_length_m_')->getString(),
                    'field_lr_stowage_height_m_'=>$reinspection_job_info->get('field_lr_stowage_height_m_')->getString(),
                    'field_lr_class_inspection'=>$reinspection_job_info->get('field_lr_class_inspection')->getString(),
                    'field_lr_mmd_inspection'=>$reinspection_job_info->get('field_lr_mmd_inspection')->getString(),
                    'field_reinspection_comments'=>$reinspection_job_info->get('field_reinspection_comments')->getString(),
                    'moderation_state' => '',
                );

            }
            else{
				  $liferaft_type_capacity=null;
                $liferaft_field_capacity=null;
                if ($liferaft->get('field_type_capacity')->first()) {

                    $liferaft_type_capacity = $liferaft->get('field_type_capacity')->first()->get('entity')->getTarget()->get('name')->getString();
                    $liferaft_field_capacity = $liferaft->get('field_type_capacity')->first()->get('entity')->getTarget()->get('field_capacity')->getString();

                }
                $date_of_prev_inspection = $liferaft->get('field_surveyed_on')->getString();
                if($date_of_prev_inspection)
                {
                    $date_of_prev_inspection1 = $liferaft->get('field_surveyed_on')->getString();
                }
                else
                {
                    $date_of_prev_inspection1 = date('Y-m-d');
                }
				 $gasCylinders = array();
                $lifedItems = array();
                $response = array(
                    'liferaft' => array(
                        'field_liferaft_date_inspection'=> date('Y-m-d'),
                        'field_liferaft_previous_inspec'=>$date_of_prev_inspection1,
                        'field_liferaft_manufacturer'=>array('title'=> $liferaft->get('field_manufacturer')->first()->get('entity')->getTarget()->get('title')->getString()),
                        'field_liferaft_serial_no'=>$liferaft->getTitle(),
                        'field_liferaft_certificate_no' => "Not Available",
                        'field_liferaft_model'=>$liferaft->get('field_liferaft_model')->getString(),
                        'field_liferaft_type' => array(
                            'type' => $liferaft_type_capacity,
                            'capacity' => $liferaft_field_capacity
                        ),
                        'field_liferaft_date_manufacturing'=>$liferaft->get('field_manufacturing_date')->getString(),
                        'field_liferaft_survivalpack_type'=>$liferaft->get('field_lr_solas_pack_type')->getString(),
                        'liferaft_id' => $liferaft->id(),
                        'vessel'=> $vessel_arr,
                    ),
					'gas_cylinders' => $gasCylinders,
                    'lifed_equipments' => $lifedItems,
                    'moderation_state' => '',
                );
            }
            return new ResourceResponse($response);
        }
    }

}