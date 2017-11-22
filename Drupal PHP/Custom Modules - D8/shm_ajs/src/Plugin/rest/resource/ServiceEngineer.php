<?php
/**
 * Created by PhpStorm.
 * User: Piyusha Pokharana
 * Date: 11/8/2017
 * Time: 11:11 AM
 */

namespace Drupal\shm_ajs\Plugin\rest\resource;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Session\AccountProxyInterface;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\node\Entity;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class ServiceEngineer
 * @package Drupal\shm_ajs\Plugin\rest\resource
 *
 * Provides a rest resource for Liferaft entity.
 *
 * @RestResource(
 *   id="service_engineer_rest",
 *   label= @Translation("ServiceEngineer Rest Resource"),
 *   serialization_class = "Drupal\shm_ajs\normalizer\JsonDenormalizer",
 *   uri_paths = {
 *    "canonical" = "/rest_api/serviceengineer/{entity}",
 *   }
 * )
 */

class ServiceEngineer extends ResourceBase{
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

    public function get($user_id=NULL){
        if($user_id){
            $user = \Drupal\user\Entity\User::load($user_id);
            $first_name = $user->get('field_first_name')->getString();
            $last_name = $user->get('field_last_name')->getString();
            $user_id = $user->get('uid')->getString();
            $full_name = $first_name.$last_name;
           // drupal_set_message("helo");
            $response = array('name' => $full_name,
                'user_id' =>$user_id,

            );
           // $response = ['message' => 'Get liferaft test'];
            return new ResourceResponse($response);
        }
    }
}