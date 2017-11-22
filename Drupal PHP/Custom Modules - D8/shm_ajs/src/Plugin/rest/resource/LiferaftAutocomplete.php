<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 27-09-2017
 * Time: 06:26 PM
 */

namespace Drupal\shm_ajs\Plugin\rest\resource;

use \Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\Plugin\ResourceInterface;
use Drupal\rest\ResourceResponse;
use Drupal\Core\Database\Database;
use Psr\Log\LoggerInterface;
use Symfony\Component\Serializer\Serializer;

/**
 * Class Liferaft
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest autocomplete callback for Liferaft entity.
 *
 * @RestResource(
 *   id="liferaft_autocomplete",
 *   label= @Translation("Liferaft Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/liferaft/autocomplete",
 *    "https://www.drupal.org/link-relations/create" = "/rest_api/liferaft/autocomplete"
 *   }
 * )
 */
class LiferaftAutocomplete extends ResourceBase {

  /**
   * Declaration of serializer object to be used for deserializing the response.
   */
  //protected $serializer;

  /**
   * LiferaftAutocomplete constructor.
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param array $serializer_formats
   * @param \Psr\Log\LoggerInterface $logger
   */
  function __construct(array $configuration, $plugin_id, $plugin_definition, array $serializer_formats, \Psr\Log\LoggerInterface $logger) {
    //$this->$serializer = new Serializer();
    parent::__construct($configuration, $plugin_id, $plugin_definition, $serializer_formats, $logger);
  }

  public function post(array $data){
    $connection = Database::getConnection();
    $search = $data['searchText'];
    $type = $data['type'];
    if($type == 'liferaft'){
       $query = $connection->select('node_field_data', 'node_data');
        $query->join('node__field_codemned_liferaft', 'condemned_lr', 'condemned_lr.entity_id = node_data.nid');
        $query->fields('node_data',array('nid','title'));
        $query->condition('node_data.status', '1', '=');
        $query->condition('node_data.type','liferafts', '=');
        $query->condition('condemned_lr.field_codemned_liferaft_value',"0",'=');
        $query->condition('title', "%$search%", 'LIKE');
        $data = $query->execute();
        $result = $data->fetchAll(\PDO::FETCH_OBJ);
        $response = array();
      foreach ($result as $row) {
          $response[] = array('nid' => $row->nid, 'title' => $row->title);
      }
    }
	
	
	
    elseif ($type == 'lr-vessel')
    {
      $query = $connection->select('node_field_data', 'node_data')
        ->fields('node_data',array('nid','title'))
        ->condition('status', '1', '=')
        ->condition('type','vessels', '=')
        ->condition('title', "%$search%", 'LIKE');
      $data = $query->execute();
      $result = $data->fetchAll(\PDO::FETCH_OBJ);
      $response = array();
      foreach ($result as $row) {
        $response[] = array('nid' => $row->nid, 'title' => $row->title);
      }

    }
    elseif ($type == 'lr-manufacturer')
    {
      $manuf =
      $query = $connection->select('node_field_data', 'node_data')
        ->fields('node_data',array('nid','title'))
        ->condition('status', '1', '=')
        ->condition('type','company', '=')
        ->condition('title', "%$search%", 'LIKE');
      $data = $query->execute();
      $result = $data->fetchAll(\PDO::FETCH_OBJ);
      $response = array();
      foreach ($result as $row) {
        $response[] = array('nid' => $row->nid, 'title' => $row->title);
      }
    }
	elseif ($type == 'liferaft-type')
    {
      $searchArray = explode('/', $search);
      $type =$searchArray[0];
      $capacity = $searchArray[1];
      $query = $connection->select('taxonomy_term_field_data', 'term_data');
      $query->join('taxonomy_term__field_capacity', 'capacity', 'term_data.tid = capacity.entity_id');
        $query->fields('term_data',array('tid','name'));
        $query->fields('capacity', array('field_capacity_value'));
        $query->condition('term_data.name',"%$type%", 'LIKE');;
      //->condition('name', "%$search%", 'LIKE');
      $data = $query->execute();
      $result = $data->fetchAll(\PDO::FETCH_OBJ);
      $response = array();
      foreach ($result as $row) {
        $response[] = array('tid' => $row->tid, 'type_capacity' => "$row->name/$row->field_capacity_value", 'type'=>$row->name, 'capacity'=>$row->field_capacity_value);
      }
    }
    elseif ($type == 'service-engineer')
    {

        $query = $connection->select('user__field_first_name', 'first_name');
        $query->join('user__field_last_name', 'last_name', 'last_name.entity_id = first_name.entity_id');
        $query->join('user__roles', 'user_role', 'user_role.entity_id = first_name.entity_id');
        $query->fields('first_name',array('field_first_name_value','entity_id'));
        $query->fields('last_name', array('field_last_name_value'));
        $query->condition('field_first_name_value', "%$search%", 'LIKE');
        $query->condition('user_role.roles_target_id','service_engineer' ,'=' );
        $data = $query->execute();
        $result = $data->fetchAll(\PDO::FETCH_OBJ);
        $response = array();
        foreach ($result as $row) {
            $response[] = array('name'=> "$row->field_first_name_value $row->field_last_name_value",'first_name' => $row->field_first_name_value, 'last_name' => $row->field_last_name_value,'uid'=> $row->entity_id);
        }
    }
/*    elseif ($type == 'lr-model-capacity')
    {
      $query = $connection->select('node_field_data', 'node_data')
        ->fields('node_data',array('nid','title'))
        ->condition('status', '1', '=')
        ->condition('type','company', '=')
        ->condition('title', "%$search%", 'LIKE');
      $data = $query->execute();
      $result = $data->fetchAll(\PDO::FETCH_OBJ);
      $response = array();
      foreach ($result as $row) {
        $response[] = array('nid' => $row->nid, 'title' => $row->title);
      }
    }
*/
    return new ResourceResponse($response);
  }
}