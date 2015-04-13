<?php

$plugin = array(
  'title' => 'Пользователь выбрал определенную роль при логине',
  'description' => 'Проверяет выбрал ли пользователь указанную роль при логине',
  'callback' => 'user_has_login_role_access_check',
  'summary' => 'user_has_login_role_access_summary',
  'settings form' => 'user_has_login_role_access_settings',
  'settings form submit' => 'user_has_login_role_access_settings_submit',
  'required context' => new ctools_context_required(t('User'), 'user'),
);

function user_has_login_role_access_settings($form, &$form_state, $conf) {
  $form['settings']['login_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Login role'),
    '#description' => t('Only the checked subsystem will be granted access.'),
    '#options' => array(),
  );

  $login_roles = gb_common_get_user_roles_info();
  foreach ($login_roles as $role_key => $info) {
    $form['settings']['login_roles']['#options'][$role_key] = $info['title'];
  }

  $form['settings']['login_roles']['#default_value'] = $conf['login_roles'];

  return $form;
}

function user_has_login_role_access_settings_submit($form, &$form_state) {
  $form_state['values']['settings']['login_roles'] = array_keys(array_filter($form_state['values']['settings']['login_roles']));
}

function user_has_login_role_access_check($conf, $context, $plugin) {
  if (empty($context)) {
    return FALSE;
  }

  $user = $context->data;
  $user_login_role = gb_common_get_login_role();

  if ($user_login_role == 'noname') {
    $user_login_role = gb_common_get_user_login_roles($user->uid);
    $user_login_role = reset($user_login_role);
  }
  elseif ($user_login_role == 'masq') {
    return TRUE;
  }

  return in_array($user_login_role, $conf['login_roles']);
}

function user_has_login_role_access_summary($conf, $context) {
  if (!isset($conf['login_roles'])) {
    $conf['login_roles'] = array();
  }
  $login_roles = gb_common_get_user_roles_info();

  $names = array();
  foreach ($conf['login_roles'] as $rid) {
    $names[] = $login_roles[$rid]['title'];
  }

  if (empty($names)) {
    return t('@identifier can have any role', array('@identifier' => $context->identifier));
  }

  return format_plural(count($names), '@identifier выбрал "@roles"', '@identifier выбрал одну из ролей "@roles" ', array('@roles' => implode(', ', $names), '@identifier' => $context->identifier));
}