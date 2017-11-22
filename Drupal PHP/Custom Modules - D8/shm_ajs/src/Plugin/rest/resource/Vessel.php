<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 14-10-2017
 * Time: 05:28 PM
 */
namespace Drupal\shm_ajs\Plugin\rest\resource;

use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Session\AccountProxyInterface;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\node\Entity;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class Liferaft
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for Liferaft entity.
 *
 * @RestResource(
 *   id="vessel_rest",
 *   label= @Translation("Vessel Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/vessel/{entity}",
 *    "https://www.drupal.org/link-relations/create" = "/rest_api/vessel/get_by_title"
 *   }
 * )
 */
class Vessel extends ResourceBase
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
      $vessel = Entity\Node::load($entity);
      $response = array('field_liferaft_vessel_imo_no' => $vessel->get('field_vessel_imo_no')->getString(),
        'field_liferaft_vesel_owner_agent' => $vessel->get('field_company')->first()->get('entity')->getTarget()->get('title')->getString(),
        'field_liferaft_vessel_call_sign'=>$vessel->get('field_vessel_call')->getString(),
        'field_liferaft_flag'=>$vessel->get('field_vessel_flag')->getString(),
        );
      return new ResourceResponse($response);
    }
  }

}