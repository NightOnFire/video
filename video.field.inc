<?php

// $Id$

/**
 * @file
 * Implement an image field, based on the file module's file field.
 */

/**
 * Implements hook_field_info().
 */
function video_field_info() {
  return array(
    'video' => array(
      'label' => t('Video'),
      'description' => t('This field stores the ID of an video file as an integer value.'),
      'settings' => array(
        'uri_scheme' => variable_get('file_default_scheme', 'public'),
        'default_video_thumbnail' => 0,
      ),
      'instance_settings' => array(
        'file_extensions' => 'mp4 ogg avi mov wmv flv',
        'file_directory' => '',
        'max_filesize' => '',
        'max_resolution' => '',
        'min_resolution' => '',
      ),
      'default_widget' => 'video_upload',
      'default_formatter' => 'video',
    ),
  );
}

/**
 * Implements hook_field_settings_form().
 */
function video_field_settings_form($field, $instance) {
  $defaults = field_info_field_settings($field['type']);
  $settings = array_merge($defaults, $field['settings']);

  $scheme_options = array();
  foreach (file_get_stream_wrappers(STREAM_WRAPPERS_WRITE_VISIBLE) as $scheme => $stream_wrapper) {
    $scheme_options[$scheme] = $stream_wrapper['name'];
  }
  $form['uri_scheme'] = array(
    '#type' => 'radios',
    '#title' => t('Upload destination'),
    '#options' => $scheme_options,
    '#default_value' => $settings['uri_scheme'],
    '#description' => t('Select where the final files should be stored. Private file storage has significantly more overhead than public files, but allows restricted access to files within this field.'),
  );

  return $form;
}

/**
 * Implements hook_field_instance_settings_form().
 */
function video_field_instance_settings_form($field, $instance) {
  $settings = $instance['settings'];

  // Use the file field instance settings form as a basis.
  $form = file_field_instance_settings_form($field, $instance);
  // Remove the description option.
  unset($form['description_field']);
  return $form;
}

/**
 * Implements hook_field_load().
 */
function video_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  file_field_load($entity_type, $entities, $field, $instances, $langcode, $items, $age);
}

/**
 * Implements hook_field_prepare_view().
 */
function video_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  // If there are no files specified at all, use the default.
  foreach ($entities as $id => $entity) {
    if (empty($items[$id]) && $field['settings']['default_video_thumbnail']) {
      if ($file = file_load($field['settings']['default_video_thumbnail'])) {
        $items[$id][0] = (array) $file + array(
          'is_default' => TRUE,
          'alt' => '',
          'title' => '',
        );
      }
    }
  }
}

/**
 * Implements hook_field_presave().
 */
function video_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  file_field_presave($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_insert().
 */
function video_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  file_field_insert($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_update().
 */
function video_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  file_field_update($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_delete().
 */
function video_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  file_field_delete($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_delete_revision().
 */
function video_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, &$items) {
  file_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_is_empty().
 */
function video_field_is_empty($item, $field) {
  return file_field_is_empty($item, $field);
}

/**
 * Implements hook_field_widget_info().
 */
function video_field_widget_info() {
  return array(
    'video_upload' => array(
      'label' => t('Video Upload'),
      'field types' => array('video'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'preview_image_style' => 'thumbnail',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
    'video_ftp' => array(
      'label' => t('Video FTP'),
      'field types' => array('video'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'preview_image_style' => 'thumbnail',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function video_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $widget['settings'];
  // Use the file widget settings form.
  $form = file_field_widget_settings_form($field, $instance);
  switch ($instance['widget']['type']) {
    case 'video_upload':
      break;
    case 'video_ftp':
      $form['ftp_path'] = array(
        '#type' => 'textfield',
        '#title' => t('FTP Filepath'),
        '#default_value' => !empty($widget['ftp_path']) ? $widget['ftp_path'] : 'ftpvideos',
        '#description' => t('The subdirectory within the "<em>files/</em>" directory where you have upload the videos for attachment.  Once the video is attached it will be moved from this directory to the main files directory.'),
        '#required' => TRUE,
        '#weight' => 3,
      );
      break;
  }
  //default settings
  $default = video_default_widget_settings($settings);
  $form = $default + $form;
  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function video_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  // Add display_field setting to field because file_field_widget_form() assumes it is set.
  $field['settings']['display_field'] = 0;

  $elements = file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  $settings = $instance['settings'];

  foreach (element_children($elements) as $delta) {
    // Add upload resolution validation.
    if ($settings['max_resolution'] || $settings['min_resolution']) {
      $elements[$delta]['#upload_validators']['file_validate_image_resolution'] = array($settings['max_resolution'], $settings['min_resolution']);
    }

    // If not using custom extension validation, ensure this is an image.
    $supported_extensions = array('png', 'gif', 'jpg', 'jpeg');
    $extensions = isset($elements[$delta]['#upload_validators']['file_validate_extensions'][0]) ? $elements[$delta]['#upload_validators']['file_validate_extensions'][0] : implode(' ', $supported_extensions);
    $extensions = array_intersect(explode(' ', $extensions), $supported_extensions);
    $elements[$delta]['#upload_validators']['file_validate_extensions'][0] = implode(' ', $extensions);

    // Add all extra functionality provided by the image widget.
    $elements[$delta]['#process'][] = 'image_field_widget_process';
  }

  if ($field['cardinality'] == 1) {
    // If there's only one field, return it as delta 0.
    if (empty($elements[0]['#default_value']['fid'])) {
      $elements[0]['#description'] = theme('file_upload_help', array('description' => $instance['description'], 'upload_validators' => $elements[0]['#upload_validators']));
    }
  } else {
    $elements['#file_upload_description'] = theme('file_upload_help', array('upload_validators' => $elements[0]['#upload_validators']));
  }
  return $elements;
}

/**
 * An element #process callback for the image_image field type.
 *
 * Expands the image_image type to include the alt and title fields.
 */
function video_field_widget_process($element, &$form_state, $form) {
  $item = $element['#value'];
  $item['fid'] = $element['fid']['#value'];

  $instance = field_widget_instance($element, $form_state);

  $settings = $instance['settings'];
  $widget_settings = $instance['widget']['settings'];

  $element['#theme'] = 'image_widget';
  $element['#attached']['css'][] = drupal_get_path('module', 'image') . '/image.css';

  // Add the image preview.
  if ($element['#file'] && $widget_settings['preview_image_style']) {
    $element['preview'] = array(
      '#type' => 'markup',
      '#markup' => theme('image_style', array('style_name' => $widget_settings['preview_image_style'], 'path' => $element['#file']->uri)),
    );
  }

  // Add the additional alt and title fields.
  $element['alt'] = array(
    '#title' => t('Alternate text'),
    '#type' => 'textfield',
    '#default_value' => isset($item['alt']) ? $item['alt'] : '',
    '#description' => t('This text will be used by screen readers, search engines, or when the image cannot be loaded.'),
    '#maxlength' => variable_get('image_alt_length', 80), // See http://www.gawds.org/show.php?contentid=28.
    '#weight' => -2,
    '#access' => (bool) $item['fid'] && $settings['alt_field'],
  );
  $element['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => isset($item['title']) ? $item['title'] : '',
    '#description' => t('The title is used as a tool tip when the user hovers the mouse over the image.'),
    '#maxlength' => variable_get('image_title_length', 500),
    '#weight' => -1,
    '#access' => (bool) $item['fid'] && $settings['title_field'],
  );

  return $element;
}

/**
 * Returns HTML for an image field widget.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: A render element representing the image field widget.
 *
 * @ingroup themeable
 */
function theme_video_widget($variables) {
  $element = $variables['element'];
  $output = '';
  $output .= '<div class="image-widget form-managed-file clearfix">';

  if (isset($element['preview'])) {
    $output .= '<div class="image-preview">';
    $output .= drupal_render($element['preview']);
    $output .= '</div>';
  }

  $output .= '<div class="image-widget-data">';
  if ($element['fid']['#value'] != 0) {
    $element['filename']['#markup'] .= ' <span class="file-size">(' . format_size($element['#file']->filesize) . ')</span> ';
  }
  $output .= drupal_render_children($element);
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function video_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $image_styles = image_style_options(FALSE);
  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
    '#options' => $image_styles,
  );

  $link_types = array(
    'content' => t('Content'),
    'file' => t('File'),
  );
  $element['image_link'] = array(
    '#title' => t('Link image to'),
    '#type' => 'select',
    '#default_value' => $settings['image_link'],
    '#empty_option' => t('Nothing'),
    '#options' => $link_types,
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function video_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = array();

  $image_styles = image_style_options(FALSE);
  // Unset possible 'No defined styles' option.
  unset($image_styles['']);
  // Styles could be lost because of enabled/disabled modules that defines
  // their styles in code.
  if (isset($image_styles[$settings['image_style']])) {
    $summary[] = t('Image style: @style', array('@style' => $image_styles[$settings['image_style']]));
  } else {
    $summary[] = t('Original image');
  }

  $link_types = array(
    'content' => t('Linked to content'),
    'file' => t('Linked to file'),
  );
  // Display this setting only if image is linked.
  if (isset($link_types[$settings['image_link']])) {
    $summary[] = $link_types[$settings['image_link']];
  }

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function video_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  // Check if the formatter involves a link.
  if ($display['settings']['image_link'] == 'content') {
    $uri = entity_uri($entity_type, $entity);
  } elseif ($display['settings']['image_link'] == 'file') {
    $link_file = TRUE;
  }

  foreach ($items as $delta => $item) {
    if (isset($link_file)) {
      $uri = array(
        'path' => file_create_url($item['uri']),
        'options' => array(),
      );
    }
    $element[$delta] = array(
      '#theme' => 'image_formatter',
      '#item' => $item,
      '#image_style' => $display['settings']['image_style'],
      '#path' => isset($uri) ? $uri : '',
    );
  }

  return $element;
}

/**
 * Returns HTML for an image field formatter.
 *
 * @param $variables
 *   An associative array containing:
 *   - item: An array of image data.
 *   - image_style: An optional image style.
 *   - path: An array containing the link 'path' and link 'options'.
 *
 * @ingroup themeable
 */
function theme_video_formatter($variables) {
  $item = $variables['item'];
  $image = array(
    'path' => $item['uri'],
    'alt' => $item['alt'],
  );
  // Do not output an empty 'title' attribute.
  if (drupal_strlen($item['title']) > 0) {
    $image['title'] = $item['title'];
  }

  if ($variables['image_style']) {
    $image['style_name'] = $variables['image_style'];
    $output = theme('image_style', $image);
  } else {
    $output = theme('image', $image);
  }

  if ($variables['path']) {
    $path = $variables['path']['path'];
    $options = $variables['path']['options'];
    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }

  return $output;
}

/**
 * Implementation of CCK's hook_field_formatter_info().
 */
function video_field_formatter_info() {
  $formatters = array(
    'video_plain' => array(
      'label' => t('Video'),
      'field types' => array('video'),
      'description' => t('Displays video files with player embedded.'),
    ),
    'video_nodelink' => array(
      'label' => t('Video Thumbnail linked to node'),
      'field types' => array('video'),
      'description' => t('Displays the video thumbnail and links to the node.'),
    ),
    //'video_colorbox' => array(
    //  'label' => t('Video Thumbnail to Colorbox'),
    //  'field types' => array('filefield'),
    //  'description' => t('Displays the video thumbnail and adds colorbox support.'),
    //),
    'video_media_js' => array(
      'label' => t('Video inject with jMedia'),
      'field types' => array('video'),
      'description' => t('Displays the video by using jmedia javascript.'),
    ),
  );
  //setup our imagecache presets
  if (module_exists('imagecache')) {
    //we need formatters for each of our thumbnails.
    $thumb_types = array('video_nodelink'); //array('video_colorbox', 'video_nodelink');
    foreach ($thumb_types as $types) {
      foreach (imagecache_presets () as $preset) {
        $formatters[$preset['presetname'] . '__' . $types] = array(
          'label' => t('@preset @label', array('@preset' => $preset['presetname'], '@label' => $formatters[$types]['label'])),
          'field types' => array('video'),
        );
      }
    }
  }
  return $formatters;
}